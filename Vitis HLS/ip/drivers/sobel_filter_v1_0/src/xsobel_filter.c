// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Tool Version Limit: 2019.12
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xsobel_filter.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XSobel_filter_CfgInitialize(XSobel_filter *InstancePtr, XSobel_filter_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->Control_r_BaseAddress = ConfigPtr->Control_r_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XSobel_filter_Start(XSobel_filter *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSobel_filter_ReadReg(InstancePtr->Control_BaseAddress, XSOBEL_FILTER_CONTROL_ADDR_AP_CTRL) & 0x80;
    XSobel_filter_WriteReg(InstancePtr->Control_BaseAddress, XSOBEL_FILTER_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XSobel_filter_IsDone(XSobel_filter *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSobel_filter_ReadReg(InstancePtr->Control_BaseAddress, XSOBEL_FILTER_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XSobel_filter_IsIdle(XSobel_filter *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSobel_filter_ReadReg(InstancePtr->Control_BaseAddress, XSOBEL_FILTER_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XSobel_filter_IsReady(XSobel_filter *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSobel_filter_ReadReg(InstancePtr->Control_BaseAddress, XSOBEL_FILTER_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XSobel_filter_EnableAutoRestart(XSobel_filter *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSobel_filter_WriteReg(InstancePtr->Control_BaseAddress, XSOBEL_FILTER_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XSobel_filter_DisableAutoRestart(XSobel_filter *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSobel_filter_WriteReg(InstancePtr->Control_BaseAddress, XSOBEL_FILTER_CONTROL_ADDR_AP_CTRL, 0);
}

void XSobel_filter_Set_threshold(XSobel_filter *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSobel_filter_WriteReg(InstancePtr->Control_BaseAddress, XSOBEL_FILTER_CONTROL_ADDR_THRESHOLD_DATA, Data);
}

u32 XSobel_filter_Get_threshold(XSobel_filter *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSobel_filter_ReadReg(InstancePtr->Control_BaseAddress, XSOBEL_FILTER_CONTROL_ADDR_THRESHOLD_DATA);
    return Data;
}

void XSobel_filter_Set_input_image(XSobel_filter *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSobel_filter_WriteReg(InstancePtr->Control_r_BaseAddress, XSOBEL_FILTER_CONTROL_R_ADDR_INPUT_IMAGE_DATA, (u32)(Data));
    XSobel_filter_WriteReg(InstancePtr->Control_r_BaseAddress, XSOBEL_FILTER_CONTROL_R_ADDR_INPUT_IMAGE_DATA + 4, (u32)(Data >> 32));
}

u64 XSobel_filter_Get_input_image(XSobel_filter *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSobel_filter_ReadReg(InstancePtr->Control_r_BaseAddress, XSOBEL_FILTER_CONTROL_R_ADDR_INPUT_IMAGE_DATA);
    Data += (u64)XSobel_filter_ReadReg(InstancePtr->Control_r_BaseAddress, XSOBEL_FILTER_CONTROL_R_ADDR_INPUT_IMAGE_DATA + 4) << 32;
    return Data;
}

void XSobel_filter_Set_output_image(XSobel_filter *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSobel_filter_WriteReg(InstancePtr->Control_r_BaseAddress, XSOBEL_FILTER_CONTROL_R_ADDR_OUTPUT_IMAGE_DATA, (u32)(Data));
    XSobel_filter_WriteReg(InstancePtr->Control_r_BaseAddress, XSOBEL_FILTER_CONTROL_R_ADDR_OUTPUT_IMAGE_DATA + 4, (u32)(Data >> 32));
}

u64 XSobel_filter_Get_output_image(XSobel_filter *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XSobel_filter_ReadReg(InstancePtr->Control_r_BaseAddress, XSOBEL_FILTER_CONTROL_R_ADDR_OUTPUT_IMAGE_DATA);
    Data += (u64)XSobel_filter_ReadReg(InstancePtr->Control_r_BaseAddress, XSOBEL_FILTER_CONTROL_R_ADDR_OUTPUT_IMAGE_DATA + 4) << 32;
    return Data;
}

void XSobel_filter_InterruptGlobalEnable(XSobel_filter *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSobel_filter_WriteReg(InstancePtr->Control_BaseAddress, XSOBEL_FILTER_CONTROL_ADDR_GIE, 1);
}

void XSobel_filter_InterruptGlobalDisable(XSobel_filter *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSobel_filter_WriteReg(InstancePtr->Control_BaseAddress, XSOBEL_FILTER_CONTROL_ADDR_GIE, 0);
}

void XSobel_filter_InterruptEnable(XSobel_filter *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XSobel_filter_ReadReg(InstancePtr->Control_BaseAddress, XSOBEL_FILTER_CONTROL_ADDR_IER);
    XSobel_filter_WriteReg(InstancePtr->Control_BaseAddress, XSOBEL_FILTER_CONTROL_ADDR_IER, Register | Mask);
}

void XSobel_filter_InterruptDisable(XSobel_filter *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XSobel_filter_ReadReg(InstancePtr->Control_BaseAddress, XSOBEL_FILTER_CONTROL_ADDR_IER);
    XSobel_filter_WriteReg(InstancePtr->Control_BaseAddress, XSOBEL_FILTER_CONTROL_ADDR_IER, Register & (~Mask));
}

void XSobel_filter_InterruptClear(XSobel_filter *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XSobel_filter_WriteReg(InstancePtr->Control_BaseAddress, XSOBEL_FILTER_CONTROL_ADDR_ISR, Mask);
}

u32 XSobel_filter_InterruptGetEnabled(XSobel_filter *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XSobel_filter_ReadReg(InstancePtr->Control_BaseAddress, XSOBEL_FILTER_CONTROL_ADDR_IER);
}

u32 XSobel_filter_InterruptGetStatus(XSobel_filter *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XSobel_filter_ReadReg(InstancePtr->Control_BaseAddress, XSOBEL_FILTER_CONTROL_ADDR_ISR);
}

