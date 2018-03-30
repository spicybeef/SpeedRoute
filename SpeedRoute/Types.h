//
//  Types.h
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-24.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#ifndef Types_h
#define Types_h

// Global types to be shared

// Direction enum
typedef enum
{
    DIR_NORTH = 0,
    DIR_EAST,
    DIR_SOUTH,
    DIR_WEST,
    DIR_NUM
} dir_e;

// Graph struct
typedef struct
{
    int * vertexArrayPointer;
    int * edgeArrayPointer;
    int vertexArraySize;
    int edgeArraySize;
} graphData_t;

#endif /* Types_h */
