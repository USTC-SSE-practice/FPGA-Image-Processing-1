// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Tool Version Limit: 2019.12
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xsobel_filter.h"

extern XSobel_filter_Config XSobel_filter_ConfigTable[];

XSobel_filter_Config *XSobel_filter_LookupConfig(u16 DeviceId) {
	XSobel_filter_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XSOBEL_FILTER_NUM_INSTANCES; Index++) {
		if (XSobel_filter_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XSobel_filter_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XSobel_filter_Initialize(XSobel_filter *InstancePtr, u16 DeviceId) {
	XSobel_filter_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XSobel_filter_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XSobel_filter_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

