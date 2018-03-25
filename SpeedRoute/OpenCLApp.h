//
//  OpenCLApp.h
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-24.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#ifndef OpenCLApp_h
#define OpenCLApp_h

#include <stdio.h>

// OpenCL Includes
#include <OpenCL/opencl.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
    
void OpenCL_PrintDeviceInfo(cl_device_id device);
void OpenCL_DeviceWalk(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
    
#endif /* OpenCLApp_h */
