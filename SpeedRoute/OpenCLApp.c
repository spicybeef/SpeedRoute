//
//  OpenCLApp.c
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-24.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#include "OpenCLApp.h"

void OpenCL_PrintDeviceInfo(cl_device_id device)
{
    char name[128];
    char vendor[128];
    
    clGetDeviceInfo(device, CL_DEVICE_NAME, 128, name, NULL);
    clGetDeviceInfo(device, CL_DEVICE_VENDOR, 128, vendor, NULL);
    fprintf(stdout, "%s : %s\n", vendor, name);
}

void OpenCL_DeviceWalk(void)
{
    size_t length;
    cl_device_id devices[8];
    int i, numDevices;
    
    cl_context context = gcl_get_context();
    
    clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(devices), devices, &length);
    
    fprintf(stdout, "The following devices are available for use\n");
    numDevices = (int)(length /sizeof(cl_device_id));
    for(i = 0; i < numDevices; i++)
    {
        OpenCL_PrintDeviceInfo(devices[i]);
    }
}
