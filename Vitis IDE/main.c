#include "xparameters.h"
#include "xuartps.h"
#include "xil_printf.h"
#include "xsobel_filter.h"
#include "xil_cache.h"
#include "xil_exception.h"

#define UART_DEVICE_ID XPAR_XUARTPS_0_DEVICE_ID
#define SOBEL_BASEADDR XPAR_SOBEL_FILTER_0_S_AXI_CONTROL_BASEADDR
#define IMG_SIZE 784 // 28x28
#define BAUDRATE 115200

#pragma align 4
static float input_image[IMG_SIZE];
#pragma align 4
static float output_image[IMG_SIZE];


int init_uart() {
    XUartPs_Config *Config = XUartPs_LookupConfig(UART_DEVICE_ID);
    if (Config == NULL) return XST_FAILURE;

    if (XUartPs_CfgInitialize(&Uart, Config, Config->BaseAddress) != XST_SUCCESS)
        return XST_FAILURE;

    XUartPs_SetBaudRate(&Uart, BAUDRATE);
    return XST_SUCCESS;
}

void process_image() {
    Xil_DCacheFlushRange((u32)input_image, IMG_SIZE * sizeof(float));

    Xil_Out32(SOBEL_BASEADDR + 0x10, (u32)input_image);  // input_addr
    Xil_Out32(SOBEL_BASEADDR + 0x1C, (u32)output_image); // output_addr
    Xil_Out32(SOBEL_BASEADDR + 0x40, 0x3D4CCCCD);        // threshold=0.2f

    // Start IP
    Xil_Out32(SOBEL_BASEADDR, 0x1);

    // Wait for completion
    while(!(Xil_In32(SOBEL_BASEADDR) & 0x2));

    // Invalidate cache for output data
    Xil_DCacheInvalidateRange((u32)output_image, IMG_SIZE * sizeof(float));
}

int main() {
    // Initialize UART
    if (init_uart() != XST_SUCCESS) {
        xil_printf("UART init failed\r\n");
        return XST_FAILURE;
    }

    xil_printf("System Ready\r\n");

    while(1) {
        // 1. Receive image (blocking)
        u8* rx_buf = (u8*)input_image;
        int remaining = IMG_SIZE * sizeof(float);

        while(remaining > 0) {
            int received = XUartPs_Recv(&Uart, rx_buf, remaining);
            if (received > 0) {
                rx_buf += received;
                remaining -= received;
            }
        }

        // 2. Process image
        process_image();

        // 3. Send result back
        u8* tx_buf = (u8*)output_image;
        remaining = IMG_SIZE * sizeof(float);

        while(remaining > 0) {
            int sent = XUartPs_Send(&Uart, tx_buf, remaining);
            if (sent > 0) {
                tx_buf += sent;
                remaining -= sent;
            }
        }
    }

    return XST_SUCCESS;
}
