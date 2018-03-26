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

// OpenCL Includes
#include <OpenCl/opencl.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
    
void OpenCl_PrintDeviceInfo(cl_device_id device);
void OpenCl_DeviceWalk(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
    
#endif /* OpenClApp_h */
