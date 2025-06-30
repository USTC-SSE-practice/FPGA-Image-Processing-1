#ifndef SOBEL_H
#define SOBEL_H

#include <ap_fixed.h>
#include <hls_stream.h>
#include <ap_int.h>
#include <hls_math.h>

// Define constants
#define IMAGE_HEIGHT 28
#define IMAGE_WIDTH 28
#define KERNEL_SIZE 3

// Optimized data types for XC7Z020 - reduced precision for better resource utilization
typedef ap_fixed<10, 4> pixel_t;       // 10 bits total, 4 integer bits
typedef ap_fixed<16, 8> gradient_t;    // 16 bits for gradient calculations
typedef ap_fixed<10, 4> result_t;      // 10 bits for final results

// Use simple arrays instead of struct for better AXI compatibility
typedef pixel_t pixel_data[IMAGE_HEIGHT * IMAGE_WIDTH];
typedef result_t result_data[IMAGE_HEIGHT * IMAGE_WIDTH];

// Function declaration with optimized interface
void sobel_filter(
    pixel_data input_image,
    result_data output_image,
    ap_fixed<8, 4> threshold = 0.2  // Use ap_fixed for threshold too
);

#endif // SOBEL_H
