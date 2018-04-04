//
//  Types.h
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-24.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#ifndef Types_h
#define Types_h

// Global types and definitions to be shared

// This increases the solution space to make the routing harder
#define SPACE_ENLARGEMENT_FACTOR    10

// Debug priorities
#define PRIO_LOW                    0
#define PRIO_NORM                   1
#define PRIO_HIGH                   2
#define PRIO_DEFAULT                PRIO_HIGH

// OpenCL work size
#define GLOBAL_WORK_SIZE 1024 * 30

// Direction enum
typedef enum
{
    DIR_NORTH = 0,
    DIR_EAST,
    DIR_SOUTH,
    DIR_WEST,
    DIR_NUM
} dir_e;

// Position struct
typedef struct
{
    int col;
    int row;
} posStruct_t;

// Graph struct
typedef struct
{
    int * vertexArrayPointer;
    int * edgeArrayPointer;
    int * blockageArrayPointer;
    int vertexArraySize;
    int edgeArraySize;
    int blockageArraySize;
    int sideLength;
} graphData_t;

// Net struct
typedef struct
{
    int * netIdArrayPointer;
    int * netVertexArrayPointer;
    int netIdArraySize;
    int netVertexArraySize;
} netData_t;

#endif /* Types_h */
