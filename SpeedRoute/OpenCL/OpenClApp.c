//
//  OpenCLApp.c
//  SpeedRoute
//
//  Copyright 1993-2009 NVIDIA Corporation.  All rights reserved.
//  Modified by Mark Zwolinski, December 2009
//  Modified by Robert McGibbon, August 2013
//  Modified by Michel Kakulphimp for SpeedRoute, March 2018
//
//

#include "OpenClApp.h"
#include "GraphWalk_WavefrontVisit.cl.h"

static dispatch_queue_t g_queue;

// Read only graph arrays
static void * d_vertexArray;
static void * d_edgeArray;
// Input and output wavefront arrays
static void * d_maskArray;
static void * d_traceArrayIn;
static void * d_traceArrayOut;
// Output wavefront data
static void * d_sinkFoundOut;
static void * d_sinkVertexOut;

void OpenCl_PrintDeviceInfo(cl_device_id device)
{
    char device_string[1024];
    
    // CL_DEVICE_NAME
    clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(device_string), &device_string, NULL);
    printf("  CL_DEVICE_NAME: \t\t\t%s\n", device_string);
    
    // CL_DEVICE_VENDOR
    clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(device_string), &device_string, NULL);
    printf("  CL_DEVICE_VENDOR: \t\t\t%s\n", device_string);
    
    // CL_DRIVER_VERSION
    clGetDeviceInfo(device, CL_DRIVER_VERSION, sizeof(device_string), &device_string, NULL);
    printf("  CL_DRIVER_VERSION: \t\t\t%s\n", device_string);
    
    // CL_DEVICE_INFO
    cl_device_type type;
    clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(type), &type, NULL);
    if( type & CL_DEVICE_TYPE_CPU )
        printf("  CL_DEVICE_TYPE:\t\t\t%s\n", "CL_DEVICE_TYPE_CPU");
    if( type & CL_DEVICE_TYPE_GPU )
        printf("  CL_DEVICE_TYPE:\t\t\t%s\n", "CL_DEVICE_TYPE_GPU");
    if( type & CL_DEVICE_TYPE_ACCELERATOR )
        printf("  CL_DEVICE_TYPE:\t\t\t%s\n", "CL_DEVICE_TYPE_ACCELERATOR");
    if( type & CL_DEVICE_TYPE_DEFAULT )
        printf("  CL_DEVICE_TYPE:\t\t\t%s\n", "CL_DEVICE_TYPE_DEFAULT");
    
    // CL_DEVICE_MAX_COMPUTE_UNITS
    cl_uint compute_units;
    clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(compute_units), &compute_units, NULL);
    printf("  CL_DEVICE_MAX_COMPUTE_UNITS:\t\t%u\n", compute_units);
    
    // CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS
    size_t workitem_dims;
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(workitem_dims), &workitem_dims, NULL);
    printf("  CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS:\t%u\n", (unsigned int)workitem_dims);
    
    // CL_DEVICE_MAX_WORK_ITEM_SIZES
    size_t workitem_size[3];
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(workitem_size), &workitem_size, NULL);
    printf("  CL_DEVICE_MAX_WORK_ITEM_SIZES:\t%u / %u / %u \n", (unsigned int)workitem_size[0], (unsigned int)workitem_size[1], (unsigned int)workitem_size[2]);
    
    // CL_DEVICE_MAX_WORK_GROUP_SIZE
    size_t workgroup_size;
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(workgroup_size), &workgroup_size, NULL);
    printf("  CL_DEVICE_MAX_WORK_GROUP_SIZE:\t%u\n", (unsigned int)workgroup_size);
    
    // CL_DEVICE_MAX_CLOCK_FREQUENCY
    cl_uint clock_frequency;
    clGetDeviceInfo(device, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(clock_frequency), &clock_frequency, NULL);
    printf("  CL_DEVICE_MAX_CLOCK_FREQUENCY:\t%u MHz\n", clock_frequency);
    
    // CL_DEVICE_ADDRESS_BITS
    cl_uint addr_bits;
    clGetDeviceInfo(device, CL_DEVICE_ADDRESS_BITS, sizeof(addr_bits), &addr_bits, NULL);
    printf("  CL_DEVICE_ADDRESS_BITS:\t\t%u\n", addr_bits);
    
    // CL_DEVICE_MAX_MEM_ALLOC_SIZE
    cl_ulong max_mem_alloc_size;
    clGetDeviceInfo(device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(max_mem_alloc_size), &max_mem_alloc_size, NULL);
    printf("  CL_DEVICE_MAX_MEM_ALLOC_SIZE:\t\t%u MByte\n", (unsigned int)(max_mem_alloc_size / (1024 * 1024)));
    
    // CL_DEVICE_GLOBAL_MEM_SIZE
    cl_ulong mem_size;
    clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(mem_size), &mem_size, NULL);
    printf("  CL_DEVICE_GLOBAL_MEM_SIZE:\t\t%u MByte\n", (unsigned int)(mem_size / (1024 * 1024)));
    
    // CL_DEVICE_ERROR_CORRECTION_SUPPORT
    cl_bool error_correction_support;
    clGetDeviceInfo(device, CL_DEVICE_ERROR_CORRECTION_SUPPORT, sizeof(error_correction_support), &error_correction_support, NULL);
    printf("  CL_DEVICE_ERROR_CORRECTION_SUPPORT:\t%s\n", error_correction_support == CL_TRUE ? "yes" : "no");
    
    // CL_DEVICE_LOCAL_MEM_TYPE
    cl_device_local_mem_type local_mem_type;
    clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_TYPE, sizeof(local_mem_type), &local_mem_type, NULL);
    printf("  CL_DEVICE_LOCAL_MEM_TYPE:\t\t%s\n", local_mem_type == 1 ? "local" : "global");
    
    // CL_DEVICE_LOCAL_MEM_SIZE
    clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(mem_size), &mem_size, NULL);
    printf("  CL_DEVICE_LOCAL_MEM_SIZE:\t\t%u KByte\n", (unsigned int)(mem_size / 1024));
    
    // CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE
    clGetDeviceInfo(device, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(mem_size), &mem_size, NULL);
    printf("  CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE:\t%u KByte\n", (unsigned int)(mem_size / 1024));
    
    // CL_DEVICE_QUEUE_PROPERTIES
    cl_command_queue_properties queue_properties;
    clGetDeviceInfo(device, CL_DEVICE_QUEUE_PROPERTIES, sizeof(queue_properties), &queue_properties, NULL);
    if( queue_properties & CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE )
        printf("  CL_DEVICE_QUEUE_PROPERTIES:\t\t%s\n", "CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE");
    if( queue_properties & CL_QUEUE_PROFILING_ENABLE )
        printf("  CL_DEVICE_QUEUE_PROPERTIES:\t\t%s\n", "CL_QUEUE_PROFILING_ENABLE");
    
    // CL_DEVICE_IMAGE_SUPPORT
    cl_bool image_support;
    clGetDeviceInfo(device, CL_DEVICE_IMAGE_SUPPORT, sizeof(image_support), &image_support, NULL);
    printf("  CL_DEVICE_IMAGE_SUPPORT:\t\t%u\n", image_support);
    
    // CL_DEVICE_MAX_READ_IMAGE_ARGS
    cl_uint max_read_image_args;
    clGetDeviceInfo(device, CL_DEVICE_MAX_READ_IMAGE_ARGS, sizeof(max_read_image_args), &max_read_image_args, NULL);
    printf("  CL_DEVICE_MAX_READ_IMAGE_ARGS:\t%u\n", max_read_image_args);
    
    // CL_DEVICE_MAX_WRITE_IMAGE_ARGS
    cl_uint max_write_image_args;
    clGetDeviceInfo(device, CL_DEVICE_MAX_WRITE_IMAGE_ARGS, sizeof(max_write_image_args), &max_write_image_args, NULL);
    printf("  CL_DEVICE_MAX_WRITE_IMAGE_ARGS:\t%u\n", max_write_image_args);
    
    // CL_DEVICE_IMAGE2D_MAX_WIDTH, CL_DEVICE_IMAGE2D_MAX_HEIGHT, CL_DEVICE_IMAGE3D_MAX_WIDTH, CL_DEVICE_IMAGE3D_MAX_HEIGHT, CL_DEVICE_IMAGE3D_MAX_DEPTH
    size_t szMaxDims[5];
    printf("\n  CL_DEVICE_IMAGE <dim>");
    clGetDeviceInfo(device, CL_DEVICE_IMAGE2D_MAX_WIDTH, sizeof(size_t), &szMaxDims[0], NULL);
    printf("\t\t\t2D_MAX_WIDTH\t %u\n", (unsigned int)szMaxDims[0]);
    clGetDeviceInfo(device, CL_DEVICE_IMAGE2D_MAX_HEIGHT, sizeof(size_t), &szMaxDims[1], NULL);
    printf("\t\t\t\t\t2D_MAX_HEIGHT\t %u\n", (unsigned int)szMaxDims[1]);
    clGetDeviceInfo(device, CL_DEVICE_IMAGE3D_MAX_WIDTH, sizeof(size_t), &szMaxDims[2], NULL);
    printf("\t\t\t\t\t3D_MAX_WIDTH\t %u\n", (unsigned int)szMaxDims[2]);
    clGetDeviceInfo(device, CL_DEVICE_IMAGE3D_MAX_HEIGHT, sizeof(size_t), &szMaxDims[3], NULL);
    printf("\t\t\t\t\t3D_MAX_HEIGHT\t %u\n", (unsigned int)szMaxDims[3]);
    clGetDeviceInfo(device, CL_DEVICE_IMAGE3D_MAX_DEPTH, sizeof(size_t), &szMaxDims[4], NULL);
    printf("\t\t\t\t\t3D_MAX_DEPTH\t %u\n", (unsigned int)szMaxDims[4]);
    
    // CL_DEVICE_PREFERRED_VECTOR_WIDTH_<type>
    printf("  CL_DEVICE_PREFERRED_VECTOR_WIDTH_<t>\t");
    cl_uint vec_width [6];
    clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, sizeof(cl_uint), &vec_width[0], NULL);
    clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, sizeof(cl_uint), &vec_width[1], NULL);
    clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, sizeof(cl_uint), &vec_width[2], NULL);
    clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, sizeof(cl_uint), &vec_width[3], NULL);
    clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, sizeof(cl_uint), &vec_width[4], NULL);
    clGetDeviceInfo(device, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, sizeof(cl_uint), &vec_width[5], NULL);
    printf("CHAR %u, SHORT %u, INT %u, FLOAT %u, DOUBLE %u\n\n",
           vec_width[0], vec_width[1], vec_width[2], vec_width[3], vec_width[4]);
}

void OpenCl_DeviceWalk(void)
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
        printf("Device ID %d:\n", i);
        OpenCl_PrintDeviceInfo(devices[i]);
    }
}

bool OpenCl_Init(unsigned int deviceId)
{
    size_t length;
    cl_device_id devices[8];
    cl_context context = gcl_get_context();
    clGetContextInfo(context, CL_CONTEXT_DEVICES, sizeof(devices), devices, &length);
    int numDevices = (int)(length /sizeof(cl_device_id));
    
    if(deviceId >= numDevices)
    {
        printf("FATAL: Invalid OpenCL device ID!\n!");
        return false;
    }
    else
    {
        printf("Will use device ID %d for OpenCL\n", deviceId);
        printf("Device information:\n");
        OpenCl_PrintDeviceInfo(devices[deviceId]);
    }
    
    g_queue = gcl_create_dispatch_queue(CL_DEVICE_TYPE_USE_ID, devices[deviceId]);
    
    return true;
}

void OpenCl_GraphWalk_InitGraphArrays(int * vertexArray, int * edgeArray, int vertexArraySize, int edgeArraySize)
{
    // Graph arrays are read only
    d_vertexArray = gcl_malloc(sizeof(cl_int) * vertexArraySize, vertexArray, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    d_edgeArray = gcl_malloc(sizeof(cl_int) * edgeArraySize, edgeArray, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
}

void OpenCl_GraphWalk_InitWavefrontData(int vertexArraySize)
{
    // Input and output wavefront arrays
    d_maskArray = gcl_malloc(sizeof(cl_int) * vertexArraySize, NULL, CL_MEM_READ_WRITE);
    d_traceArrayIn = gcl_malloc(sizeof(cl_int) * vertexArraySize, NULL, CL_MEM_READ_ONLY);
    d_traceArrayOut = gcl_malloc(sizeof(cl_int) * vertexArraySize, NULL, CL_MEM_WRITE_ONLY);
    // Output wavefront data
    d_sinkFoundOut = gcl_malloc(sizeof(cl_int), NULL, CL_MEM_WRITE_ONLY);
    d_sinkVertexOut = gcl_malloc(sizeof(cl_int), NULL, CL_MEM_WRITE_ONLY);
}

void OpenCl_GraphWalk_FreeWavefrontData(void)
{
    // Input and output wavefront arrays
    gcl_free(d_maskArray);
    gcl_free(d_traceArrayIn);
    gcl_free(d_traceArrayOut);
    // Output wavefront data
    gcl_free(d_sinkFoundOut);
    gcl_free(d_sinkVertexOut);
}

void OpenCl_GraphWalk_SetWavefrontData(int * maskArray, int * traceArray, int vertexArraySize)
{
    dispatch_sync(g_queue, ^{
        // Copy mask array
        gcl_memcpy(d_maskArray, maskArray, sizeof(cl_int) * vertexArraySize);
        // Copy trace array
        gcl_memcpy(d_traceArrayIn, traceArray, sizeof(cl_int) * vertexArraySize);
    });
}

void OpenCl_GraphWalk_GetWavefrontData(int * maskArray, int vertexArraySize, bool * sinkFound, int * sinkVertexId)
{
    int sinkFoundInt;
    int * sinkFoundIntPointer = &sinkFoundInt;
    
    dispatch_sync(g_queue, ^{
        // Copy mask array
        gcl_memcpy(maskArray, d_maskArray, sizeof(cl_int) * vertexArraySize);
        // Copy the sinkFound flag
        gcl_memcpy(sinkFoundIntPointer, d_sinkFoundOut, sizeof(cl_int));
        // Copy the sink vertex ID regardless
        gcl_memcpy(sinkVertexId, d_sinkVertexOut, sizeof(cl_int));
    });
    
    if(sinkFoundInt == 1)
    {
        *sinkFound = true;
    }
    
//    printf("OpenCL: sinkFoundInt: %d\n", sinkFoundInt);
//    printf("OpenCL: sinkVertexId: %d\n", *sinkVertexId);
}

void OpenCl_GraphWalk_WavefrontVisit(bool firstNetVertex, int vertexArraySize)
{
    cl_int firstNetVertexInt;
    
    // Change the bool to an OpenCL friendly int
    if(firstNetVertex)
    {
        firstNetVertexInt = 1;
    }
    else
    {
        firstNetVertexInt = 0;
    }
    
    // We'll use a semaphore to synchronize the host and OpenCL device.
    dispatch_semaphore_t dsema = dispatch_semaphore_create(0);
    
    // OpenCl_GraphWalk_GetWavefrontData should have been called by this point, ready for dispatch
    // Note that this will execute asynchronously with respect
    // to the host application.
    dispatch_async(g_queue, ^{

        cl_ndrange range = {
            1,                      // We're using a 1-dimensional execution.
            {0},                    // Start at the beginning of the range.
            {vertexArraySize},      // Execute 'vertexArraySize' work items.
            {0}                     // Let OpenCL decide how to divide work items
                                    // into workgroups.
        };
        
        GraphWalk_WavefrontVisit_kernel(
                                        &range,
                                        (cl_int*)d_vertexArray,
                                        (cl_int*)d_edgeArray,
                                        (cl_int*)d_maskArray,
                                        (cl_int*)d_traceArrayIn,
                                        (cl_int*)d_sinkFoundOut,
                                        (cl_int*)d_sinkVertexOut,
                                        firstNetVertexInt,
                                        vertexArraySize);

        // Okay -- signal the dispatch semaphore so the host knows
        // it can continue.
        dispatch_semaphore_signal(dsema);
    });
    
    // Here the host could do other, unrelated work while the OpenCL
    // device works on the kernel-based computation...
    // But now we wait for OpenCL to finish up.
    dispatch_semaphore_wait(dsema, DISPATCH_TIME_FOREVER);
}
