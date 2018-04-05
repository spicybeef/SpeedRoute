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
#define SPACE_ENLARGEMENT_FACTOR    1

// Debug priorities
#define PRIO_LOW                    0
#define PRIO_NORM                   1
#define PRIO_HIGH                   2
#define PRIO_DEFAULT                PRIO_HIGH

// Default input files
#define DEBUG_TO_BENCHMARKS_FOLDER_PATH "../../../../../benchmarks/"
//#define DEFAULT_NET_FILE "apex4.txt"
//#define DEFAULT_PLACEMENT_FILE "apex4_placement.txt"
#define DEFAULT_NET_FILE "test.txt"
#define DEFAULT_PLACEMENT_FILE "test_placement.txt"
//#define DEFAULT_NET_FILE "cm138a.txt"
//#define DEFAULT_PLACEMENT_FILE "cm138a_placement.txt"

extern int g_debugLevel;
extern int g_enlargementFactor;

// Graphics constants
#define WIN_VIEWPORT_WIDTH                      1000.f
#define WIN_VIEWPORT_HEIGHT                     800.f
#define WIN_INFOPORT_WIDTH                      WIN_VIEWPORT_WIDTH
#define WIN_INFOPORT_HEIGHT                     100.f
#define WIN_GRAPHICPORT_WIDTH                   WIN_VIEWPORT_WIDTH
#define WIN_GRAPHICPORT_HEIGHT                  (WIN_VIEWPORT_HEIGHT - WIN_INFOPORT_HEIGHT)
#define WIN_INFOPORT_PADDING                    10.f

// Grid constants
#define GRID_SHRINK_FACTOR                      0.1f
#define CELL_SHRINK_FACTOR                      0.7f

// Direction enum
typedef enum
{
    DIR_NORTH = 0,
    DIR_EAST,
    DIR_SOUTH,
    DIR_WEST,
    DIR_NUM
} dir_e;

// Col/row position struct
typedef struct
{
    unsigned int                                col;                    ///< Column
    unsigned int                                row;                    ///< Row
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

typedef struct
{
    float                                       cellSize;               ///< Current cellsize
    float                                       cellOffset;             ///< Cell offset
    float                                       cellOppositeOffset;     ///< Cell offset for other dimension
} cellPropertiesStruct_t;

typedef struct
{
    // Net ID -> Segment ID -> Vertex ID
    int * netRouteArrayPointer;
    int netRouteArraySize;
    int * segmentIdArrayPointer;
    int segmentIdArraySize;
    int * segmentVertexArrayPointer;
    int segmentVertexArraySize;
} netRoutes_t;

#endif /* Types_h */
