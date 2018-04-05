//
//  OpenCLApp.h
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-24.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#ifndef OpenClApp_h
#define OpenClApp_h

#include <stdio.h>

#include <OpenCL/opencl.h>

void OpenCl_PrintDeviceInfo(cl_device_id device);
void OpenCl_DeviceWalk(void);
bool OpenCl_Init(unsigned int deviceId);
void OpenCl_GraphWalk_InitGraphArrays(int * vertexArray, int * edgeArray, int vertexArraySize, int edgeArraySize);
void OpenCl_GraphWalk_InitWavefrontData(int vertexArraySize, int * maskArray, int * traceArray);
void OpenCl_GraphWalk_FreeAllData(void);
void OpenCl_GraphWalk_FreeWavefrontData(void);
void OpenCl_GraphWalk_SetWavefrontData(int * maskArray, int * traceArray, int vertexArraySize);
void OpenCl_GraphWalk_GetWavefrontData(int * maskArray, int vertexArraySize, bool * sinkFound, int * sinkVertexId);
void OpenCl_GraphWalk_WavefrontVisit(bool firstNetVertex, int vertexArraySize);
    
#endif /* OpenClApp_h */
