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

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else /* __APPLE__ */
#include <CL/cl.h>
#endif /*__APPLE__ */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
    
void OpenCl_PrintDeviceInfo(cl_device_id device);
void OpenCl_DeviceWalk(void);
bool OpenCl_Init(unsigned int deviceId);

#ifdef __cplusplus
}
#endif /* __cplusplus */
    
#endif /* OpenClApp_h */
