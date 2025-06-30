#include "sobel.h"

void sobel_filter(
    pixel_data input_image,
    result_data output_image,
    ap_fixed<8, 4> threshold)
{
    #pragma HLS INTERFACE m_axi port=input_image offset=slave bundle=gmem0 depth=784 max_read_burst_length=64
    #pragma HLS INTERFACE m_axi port=output_image offset=slave bundle=gmem1 depth=784 max_write_burst_length=64
    #pragma HLS INTERFACE s_axilite port=threshold bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control

    pixel_t local_input[IMAGE_HEIGHT][IMAGE_WIDTH];
    result_t local_output[IMAGE_HEIGHT][IMAGE_WIDTH];

    #pragma HLS ARRAY_PARTITION variable=local_input cyclic factor=2 dim=2
    #pragma HLS ARRAY_PARTITION variable=local_output cyclic factor=2 dim=2

    const ap_int<3> Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    const ap_int<3> Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
    #pragma HLS ARRAY_PARTITION variable=Gx complete dim=0
    #pragma HLS ARRAY_PARTITION variable=Gy complete dim=0

    COPY_INPUT:
    for (int i = 0; i < IMAGE_HEIGHT * IMAGE_WIDTH; i++) {
        #pragma HLS PIPELINE II=1
        #pragma HLS LOOP_TRIPCOUNT min=784 max=784
        int row = i / IMAGE_WIDTH;
        int col = i % IMAGE_WIDTH;
        local_input[row][col] = input_image[i];
    }

    gradient_t max_grad = 0;

    ROW_LOOP:
    for (int y = 0; y < IMAGE_HEIGHT; y++) {
        #pragma HLS LOOP_TRIPCOUNT min=28 max=28

        COL_LOOP:
        for (int x = 0; x < IMAGE_WIDTH; x++) {
            #pragma HLS PIPELINE II=2  // Relaxed II for ZedBoard
            #pragma HLS LOOP_TRIPCOUNT min=28 max=28

            // Process only valid pixels
            if (y >= 1 && y < IMAGE_HEIGHT-1 && x >= 1 && x < IMAGE_WIDTH-1) {
                gradient_t gx = 0, gy = 0;

                // Compute gradients with bounds checking
                KERNEL_Y:
                for (int i = 0; i < 3; i++) {
                    #pragma HLS UNROLL
                    KERNEL_X:
                    for (int j = 0; j < 3; j++) {
                        #pragma HLS UNROLL
                        int py = y - 1 + i;
                        int px = x - 1 + j;

                        // Safe bounds checking
                        if (py >= 0 && py < IMAGE_HEIGHT && px >= 0 && px < IMAGE_WIDTH) {
                            pixel_t pixel_val = local_input[py][px];
                            gx += pixel_val * Gx[i][j];
                            gy += pixel_val * Gy[i][j];
                        }
                    }
                }

                // Compute magnitude using abs instead of sqrt for better resource usage
                gradient_t abs_gx, abs_gy;
                if (gx < 0) abs_gx = -gx; else abs_gx = gx;
                if (gy < 0) abs_gy = -gy; else abs_gy = gy;
                gradient_t mag = abs_gx + abs_gy;  // Manhattan distance approximation

                // Track maximum gradient
                if (mag > max_grad) {
                    max_grad = mag;
                }

                local_output[y][x] = mag;
            } else {
                // Border pixels
                local_output[y][x] = 0;
            }
        }
    }

    // Normalization and thresholding
    NORM_LOOP:
    for (int i = 0; i < IMAGE_HEIGHT * IMAGE_WIDTH; i++) {
        #pragma HLS PIPELINE II=1
        #pragma HLS LOOP_TRIPCOUNT min=784 max=784
        int row = i / IMAGE_WIDTH;
        int col = i % IMAGE_WIDTH;

        if (max_grad > 0) {
            result_t norm_val = local_output[row][col] / max_grad;
            if (threshold > 0) {
                output_image[i] = (norm_val > threshold) ? result_t(1.0) : result_t(0.0);
            } else {
                output_image[i] = norm_val;
            }
        } else {
            output_image[i] = 0;
        }
    }
}
