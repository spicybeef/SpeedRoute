//
//  GraphWalk.c
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-28.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "GraphWalk.h"
#include "OpenClApp.h"
#include "Types.h"

// Module scope data

int g_debugLevel = PRIO_DEFAULT;

// Graph walk arrays
static int * g_traceArray;
static int * g_maskArray;
static int * g_weightArray;
static int * g_netStatusArray;

// Graph arrays
static int * g_vertexArray;
static int * g_edgeArray;
static int g_vertexArraySize;
static int g_edgeArraySize;

// Net arrays
// The following arrays are used to store a net's vertexes
// Hierarchy:
// Net ID -> Vertex ID
static int * g_netVertexesArrayPointer;
static int g_netVertexesArraySize;
static int * g_vertexIdArrayPointer;
static int g_vertexIdArraySize;

// Struct to store net routes
static volatile netRoutes_t g_netRoutes;

// Architecture side length
int g_sideLength;
// Architecture channel width
static int g_channelWidth;

// Routing variables
static int g_currentSourceVertex;
static int g_currentSinkVertex;
static int g_currentExpansion;
static bool g_firstNetVertex;
static bool g_sinkFound;
static int g_currentNetNum;
static routingState_e g_currentRoutingState = STATE_IDLE;
static volatile bool g_routingRunning = true;
static volatile bool g_tracingBack = false;
static volatile bool g_netRoutesLock = false;

#define VERTEX_NONETYPE         -1
#define VERTEX_BLOCK            -2
#define VERTEX_NET_UNCONN       -3
#define VERTEX_NET_CONN         -4

#define MASK_NO_VISIT           0
#define MASK_VISIT              1
#define MASK_VISIT_NEXT         2
#define MASK_VISITED            3

#define NET_NONETYPE            0
#define NET_CONNECTED           1
#define NET_UNCONNECTED         2

void GraphWalk_Test(int priority, graphData_t graph, netData_t nets)
{
    int arraySize, value;
    
    if(priority >= g_debugLevel)
    {
        arraySize = graph.vertexArraySize;
        printf("Vertex array: ");
        for(int i = 0; i < arraySize; i++)
        {
            value = graph.vertexArrayPointer[i];
            printf("%i ", value);
        }
        printf("\n");
        
        arraySize = graph.edgeArraySize;
        printf("Edge array: ");
        for(int i = 0; i < arraySize; i++)
        {
            value = graph.edgeArrayPointer[i];
            printf("%i ", value);
        }
        printf("\n");
        
        arraySize = nets.netIdArraySize;
        printf("Net ID array: ");
        for(int i = 0; i < arraySize; i++)
        {
            int value = nets.netIdArrayPointer[i];
            printf("%i ", value);
        }
        printf("\n");
        
        arraySize = nets.netVertexArraySize;
        printf("Net Vertex array: ");
        for(int i = 0; i < arraySize; i++)
        {
            int value = nets.netVertexArrayPointer[i];
            printf("%i ", value);
        }
        printf("\n");
    }
}

void GraphWalk_DebugPrint(int priority, const char * format, ... )
{
    va_list args;
    va_start(args, format);
    
    if(priority >= g_debugLevel)
    {
        vprintf(format, args);
    }
    
    va_end(args);
}

void GraphWalk_DebugPrintGrid(int priority, char * string, int * gridArray)
{
    if(priority >= g_debugLevel)
    {
        printf("%s\n", string);
        for(int row = 0; row < g_sideLength; row++)
        {
            for(int col = 0; col < g_sideLength; col++)
            {
                {
                    if(gridArray[col + row * g_sideLength] == 0 || gridArray[col + row * g_sideLength] == -1)
                    {
                        printf(" . ");
                    }
                    else if(gridArray[col + row * g_sideLength] == VERTEX_BLOCK)
                    {
                        printf(" X ");
                    }
                    else
                    {
                        printf("%2i ", gridArray[col + row * g_sideLength]);
                    }
                }
            }
            printf("\n");
        }
    }
}

void GraphWalk_DebugPrintRoutes(int priority)
{
    if(priority >= g_debugLevel)
    {
        printf("Route summary:\n");
        
        printf("Net route array: ");
        for(int i = 0; i < g_netRoutes.netRouteArraySize; i++)
        {
            int value = g_netRoutes.netRouteArrayPointer[i];
            printf("%i ", value);
        }
        printf("\n");
        
        printf("Net segment ID array: ");
        for(int i = 0; i < g_netRoutes.segmentIdArraySize; i++)
        {
            int value = g_netRoutes.segmentIdArrayPointer[i];
            printf("%i ", value);
        }
        printf("\n");
        
        printf("Net segment vertex array: ");
        for(int i = 0; i < g_netRoutes.segmentVertexArraySize; i++)
        {
            int value = g_netRoutes.segmentVertexArrayPointer[i];
            printf("%i ", value);
        }
        printf("\n");
    }
}

void GraphWalk_Main(bool clEnable)
{
    // Signal that we've started routing
    g_routingRunning = true;
    // Initialize the net status array
    GraphWalk_InitNetStatus();
    // Initiailize the weight array
    GraphWalk_InitWeight();
    // Initialize the routing arrays
    GraphWalk_InitNetRoutes();
    bool routeFailed = false;
    for(g_currentNetNum = 0; g_currentNetNum < g_netVertexesArraySize; g_currentNetNum++)
    {
        GraphWalk_DebugPrint(PRIO_HIGH, "Routing net: %d of %d\n", g_currentNetNum + 1, g_netVertexesArraySize);
        // Init a new net route
        GraphWalk_NewNetRoute();
        // Route the nets
        if(!GraphWalk_RouteNet(clEnable, g_currentNetNum))
        {
            routeFailed = true;
            break;
        }
    }
    if(routeFailed)
    {
        g_currentRoutingState = STATE_ROUTE_FAILURE;
        GraphWalk_DebugPrint(PRIO_HIGH, "ROUTE FAILURE!\n");
    }
    else
    {
        g_currentRoutingState = STATE_ROUTE_SUCCESS;
        GraphWalk_DebugPrint(PRIO_HIGH, "ROUTE SUCCESS!\n");
    }
    // No longer routing
    g_routingRunning = false;
}

routingState_e GraphWalk_GetCurrentState(void)
{
    return g_currentRoutingState;
}

int GraphWalk_GetCurrentNetNum(void)
{
    return g_currentNetNum;
}

int GraphWalk_GetTotalNetNum(void)
{
    return g_netVertexesArraySize;
}

int GraphWalk_GetCurrentSourceVertex(void)
{
    return g_currentSourceVertex;
}
int GraphWalk_GetCurrentSinkVertex(void)
{
    return g_currentSinkVertex;
}

int GraphWalk_GetCurrentExpansion(void)
{
    return g_currentExpansion;
}

bool GraphWalk_IsRoutingRunning(void)
{
    return g_routingRunning;
}

bool GraphWalk_IsTracingBack(void)
{
    return g_tracingBack;
}

void GraphWalk_LockNetSegments(bool lock)
{
    g_netRoutesLock = lock;
}

void GraphWalk_WaitLock(bool * lockVar)
{
    while(*lockVar)
    {
        // Wait...
    }
}

void GraphWalk_WaitLockNetsSegments(void)
{
    GraphWalk_WaitLock((bool*)&g_netRoutesLock);
    g_netRoutesLock = true;
}

void GraphWalk_InitWalkData(graphData_t graph, netData_t nets, int channelWidth)
{
    g_vertexArray = graph.vertexArrayPointer;
    g_edgeArray = graph.edgeArrayPointer;
    g_vertexArraySize = graph.vertexArraySize;
    g_edgeArraySize = graph.edgeArraySize;
    
    g_netVertexesArrayPointer = nets.netIdArrayPointer;
    g_vertexIdArrayPointer = nets.netVertexArrayPointer;
    g_netVertexesArraySize = nets.netIdArraySize;
    g_vertexIdArraySize = nets.netVertexArraySize;
    
    g_traceArray = malloc(graph.edgeArraySize * sizeof(int));
    g_maskArray = malloc(graph.vertexArraySize * sizeof(int));
    g_weightArray = malloc(graph.edgeArraySize * sizeof(int));
    g_netStatusArray = malloc(nets.netVertexArraySize * sizeof(int));
    
    g_sideLength = graph.sideLength;
    g_channelWidth = channelWidth;
}

void GraphWalk_FreeWalkData(void)
{
    free(g_traceArray);
    free(g_maskArray);
    free(g_weightArray);
    free(g_netStatusArray);
}

// Inits are done in a loop to be extra portable
// Hopefully the compiler is smart enough to turn this into the proper memset
void GraphWalk_InitMask(void)
{
    GraphWalk_DebugPrint(PRIO_LOW, "Initing mask...\n");
    // Init mask array to MASK_NO_VISIT
    for(int vertex = 0; vertex < g_vertexArraySize; vertex++)
    {
        g_maskArray[vertex] = MASK_NO_VISIT;
    }
}

void GraphWalk_InitTrace(void)
{
    int col, row;
    // Init trace array to either a block or nonetype
    for(int i = 0; i < g_edgeArraySize; i++)
    {
        // Ignore unconnected, connected nets, and current blocks
        if(g_traceArray[i] == VERTEX_NET_UNCONN || g_traceArray[i] == VERTEX_NET_CONN || g_traceArray[i] == VERTEX_BLOCK)
        {
            continue;
        }
        col = i % g_sideLength;
        row = i / g_sideLength;
        
        // Logic exists at odd intersections of row and col
        if((row % 2 == 1) && (col % 2 == 1))
        {
            g_traceArray[i] = VERTEX_BLOCK;
        }
        else
        {
            g_traceArray[i] = VERTEX_NONETYPE;
        }
    }
}

void GraphWalk_InitWeight(void)
{
    // Init weight array to 0
    for(int i = 0; i < g_vertexArraySize; i++)
    {
        g_weightArray[i] = 0;
    }
}

void GraphWalk_InitNetStatus(void)
{
    // Init net status array to NET_UNCONNECTED
    // We'll have to visit and connect the same about of connects as in netVertexArraySize, so use that for init
    for(int i = 0; i < g_vertexIdArraySize; i++)
    {
        g_netStatusArray[i] = NET_UNCONNECTED;
    }
}

bool GraphWalk_RouteNet(bool openCl, int netId)
{
    time_t t;
    // Seed random number
    srand((unsigned) time(&t));
    
    g_firstNetVertex = true;
    
    int netVertexIndexStart, netVertexIndexEnd, randNetVertexIndex;
    
    // Index of net ID array points to the first node of a net
    // Index + 1 of net ID array points to the first node of the next net
    // Determine the start and end of the net vertex array to index for this net ID
    netVertexIndexStart = g_netVertexesArrayPointer[netId];
    // Check if we're at the end of the array
    if ((netId + 1) < (g_netVertexesArraySize))
    {
        // We're still within bounds, end is next index
        netVertexIndexEnd = g_netVertexesArrayPointer[netId + 1];
    }
    else
    {
        // We've reached the end, the size is the end
        netVertexIndexEnd = g_vertexIdArraySize;
    }
    GraphWalk_DebugPrint(PRIO_NORM, "Net ID %d start and end index: %d %d\n", netId, netVertexIndexStart, netVertexIndexEnd);
    
    // Update trace array with current net vertexes
    for(int vertexIndex = netVertexIndexStart; vertexIndex < netVertexIndexEnd; vertexIndex++)
    {
        g_traceArray[g_vertexIdArrayPointer[vertexIndex]] = VERTEX_NET_UNCONN;
    }
    
    // Init the mask
    GraphWalk_InitMask();
    
    // Allocate some memory on the device
    OpenCl_GraphWalk_InitWavefrontData(g_vertexArraySize, g_maskArray, g_traceArray);
    
    // Route until we run out of vertexes to route for this net
    do
    {
        // Init the trace
        GraphWalk_InitTrace();
        // Pick a random vertex index to start routing from
        randNetVertexIndex = (rand() % (netVertexIndexEnd - netVertexIndexStart)) + netVertexIndexStart;
        g_currentSourceVertex = g_vertexIdArrayPointer[randNetVertexIndex];
        // Check if this one has been connected already
        if(g_netStatusArray[randNetVertexIndex] == NET_CONNECTED)
        {
            continue;
        }
        GraphWalk_DebugPrint(PRIO_NORM, "Sourcing from %d\n", g_currentSourceVertex);
        g_maskArray[g_currentSourceVertex] = MASK_VISIT;
        // This net's vertex becomes a connected source, mark it as such
        g_netStatusArray[randNetVertexIndex] = NET_CONNECTED;
        
        /* Wavefront Expansion */
        g_currentExpansion = 0;
        g_currentRoutingState = STATE_EXPANSION;
        for(;;)
        {
            // Do a wavefront visit
            g_sinkFound = false;
            
            GraphWalk_DebugPrint(PRIO_LOW, "Arrays before wavefront visit:\n");
            GraphWalk_DebugPrintGrid(PRIO_LOW, "Trace Array", g_traceArray);
            GraphWalk_DebugPrintGrid(PRIO_LOW, "Mask Array", g_maskArray);
            
            if(openCl)
            {
                GraphWalk_WavefrontVisit_Cl();
            }
            else
            {
                GraphWalk_WavefrontVisit();
            }
            
            // Print out some info
            GraphWalk_DebugPrint(PRIO_LOW, "Arrays after wavefront visit:\n");
            GraphWalk_DebugPrintGrid(PRIO_LOW, "Trace Array", g_traceArray);
            GraphWalk_DebugPrintGrid(PRIO_LOW, "Mask Array", g_maskArray);
            
            // If it's empty, check if we've found a sink
            if(g_sinkFound)
            {
                // Init the mask, we're done routing
                GraphWalk_InitMask();
                // If we're the first vertex
                if(g_firstNetVertex)
                {
                    // No longer the first vertex
                    g_firstNetVertex = false;
                }
                // Update the trace array with connected vertexes
                g_traceArray[g_currentSinkVertex] = VERTEX_NET_CONN;
                g_traceArray[g_currentSourceVertex] = VERTEX_NET_CONN;
                // Our sink is now connected, mark it as such
                for(int vertexIndex = netVertexIndexStart; vertexIndex < netVertexIndexEnd; vertexIndex++)
                {
                    if(g_vertexIdArrayPointer[vertexIndex] == g_currentSinkVertex)
                    {
                        g_netStatusArray[vertexIndex] = NET_CONNECTED;
                    }
                }
                // Sink has been found, route success
                // Sink is equivalent to the next expansion
                g_currentExpansion++;
                break;
            }
            else if(!GraphWalk_IsMaskArrayEmpty(g_maskArray, g_vertexArraySize))
            {
                // Not empty yet. Change next visits to visits
                GraphWalk_UpdateTraceAndMask();
                // Go to the next expansion!
                g_currentExpansion++;
                continue;
            }
            // No sink found, route failed
            else
            {
                GraphWalk_DebugPrintGrid(PRIO_LOW, "Trace Array", g_traceArray);
                GraphWalk_DebugPrintGrid(PRIO_LOW, "Mask Array", g_maskArray);
                GraphWalk_DebugPrint(PRIO_HIGH, "Route failed for net ID: %d!\n", netId);
                return false;
            }
            
        } /* Wavefront expansion */
        
        // We've got a new segment
        GraphWalk_NewSegment();
        
        /* Traceback */
        GraphWalk_TraceBack();
        
        // Change vertexes at capacity into blocks
        GraphWalk_UpdateBlocksFromWeight();
    }
    while(GraphWalk_IsNetUnconnected(netVertexIndexStart, netVertexIndexEnd, g_netStatusArray));
    
    GraphWalk_DebugPrintGrid(PRIO_LOW, "Weights:\n", g_weightArray);
    
    // If we're here we've successfully routed
    return true;
}

void GraphWalk_WavefrontVisit(void)
{
    // Visit masked areas
    for(int vertex = 0; vertex < g_vertexArraySize; vertex++)
    {
        // Check if we need to visit
        if(g_maskArray[vertex] == MASK_VISIT)
        {
            // We do! Turn it into a visited box
            g_maskArray[vertex] = MASK_VISITED;
            // Index of vertex array points to entry in edge array
            // Index + 1 of vertex array points to the first node of the next vertex's edges
            // Determine the start and end of the net vertex array to index for this net ID
            int start = g_vertexArray[vertex];
            int end;
            // Check if we're at the end of the array
            if ((vertex + 1) < (g_vertexArraySize))
            {
                // We're still within bounds, end is next index
                end = g_vertexArray[vertex + 1];
            }
            else
            {
                // We've reached the end, the size is the end
                end = g_vertexArraySize;
            }
            
            // Now go through the vertex's edges
            for(int edgeIndex = start; edgeIndex < end; edgeIndex++)
            {
                int nextVertex = g_edgeArray[edgeIndex];
                GraphWalk_DebugPrint(PRIO_LOW, "Visiting %d\n", nextVertex);
                
                if(g_traceArray[nextVertex] == VERTEX_BLOCK)
                {
                    // Can't go here, go to next edge
                    continue;
                }
                // Check if it's been visited
                if(g_maskArray[nextVertex] == MASK_VISITED)
                {
                    // Can't go back, go to next edge
                    continue;
                }
                // Check if it's a valid connection
                if(g_traceArray[nextVertex] == VERTEX_NET_UNCONN || g_traceArray[nextVertex] == VERTEX_NET_CONN)
                {
                    // OK, so if it's the first net vertex, we can sink to anything.
                    // Subsequent sinks MUST be already connected to avoid unconnected graphs.
                    // We run the risk of some ugly connections, but at least they'll lead to fully connected graphs.
                    if(!g_firstNetVertex)
                    {
                        // We've started connecting nets already, ignore unconnected ones
                        if(g_traceArray[nextVertex] == VERTEX_NET_UNCONN)
                        {
                            continue;
                        }
                    }
                    // Stop the presses, we've found a sink
                    g_sinkFound = true;
                    GraphWalk_DebugPrint(PRIO_LOW, "Found sink @ %d!\n", nextVertex);
                    // We're going to trace back from here
                    g_currentSinkVertex = nextVertex;
                    break;
                }
                g_maskArray[nextVertex] = MASK_VISIT_NEXT;
            }
        }
    }
}

void GraphWalk_WavefrontVisit_Cl(void)
{
    int sinkVertex;
    bool sinkFound = false;
    // Transfer relevant data to the device
    GraphWalk_DebugPrint(PRIO_LOW, "OpenCL: Copying wavefront data\n");
    OpenCl_GraphWalk_SetWavefrontData(g_maskArray, g_traceArray, g_vertexArraySize);
    // Run the kernel (this function is blocking)
    GraphWalk_DebugPrint(PRIO_LOW, "OpenCL: Running wavefront kernel\n");
    OpenCl_GraphWalk_WavefrontVisit(g_firstNetVertex, g_vertexArraySize);
    // Get the data back
    GraphWalk_DebugPrint(PRIO_LOW, "OpenCL: Getting wavefront data\n");
    OpenCl_GraphWalk_GetWavefrontData(g_maskArray, g_vertexArraySize, &sinkFound, &sinkVertex);
    // Free memory on the device
    OpenCl_GraphWalk_FreeWavefrontData();
    
    if(sinkFound)
    {
        g_sinkFound = true;
        g_currentSinkVertex = sinkVertex;
        GraphWalk_DebugPrint(PRIO_LOW, "OpenCL: Found sink @ %d\n", sinkVertex);
    }
}

void GraphWalk_UpdateTraceAndMask(void)
{
    // This is required to synchronize the work done previously, preventing visiting things we just marked for visitation
    for(int vertex = 0; vertex < g_vertexArraySize; vertex++)
    {
        // Check if we need to visit
        if(g_maskArray[vertex] == MASK_VISIT_NEXT)
        {
            g_traceArray[vertex] = g_currentExpansion + 1;
            g_maskArray[vertex] = MASK_VISIT;
        }
    }
}

void GraphWalk_TraceBack(void)
{
    /* Traceback */
    g_tracingBack = true;
    g_currentRoutingState = STATE_TRACEBACK;
    GraphWalk_DebugPrint(PRIO_LOW, "Tracing back from %d\n", g_currentSinkVertex);
    GraphWalk_SegmentAppend(g_currentSinkVertex);
    bool foundSource = false;
    do
    {
        // Determine the start and end of the net vertex array to index for this net ID
        int start = g_vertexArray[g_currentSinkVertex];
        int end;
        // Check if we're at the end of the array
        if ((g_currentSinkVertex + 1) < (g_vertexArraySize))
        {
            // We're still within bounds, end is next index
            end = g_vertexArray[g_currentSinkVertex + 1];
        }
        else
        {
            // We've reached the end, the size is the end
            end = g_vertexArraySize;
        }
        // Now go through the vertex's edges
        for(int edgeIndex = start; edgeIndex < end; edgeIndex++)
        {
            int nextVertex = g_edgeArray[edgeIndex];
            // Check if it's our source
            if(nextVertex == g_currentSourceVertex)
            {
                GraphWalk_DebugPrint(PRIO_NORM, "Found the original source @ %d\n", nextVertex);
                GraphWalk_SegmentAppend(nextVertex);
                foundSource = true;
                break;
            }
            // Check if it's a route back
            if(g_traceArray[nextVertex] == g_currentExpansion - 1)
            {
                GraphWalk_DebugPrint(PRIO_NORM, "Found a way back through %d\n", nextVertex);
                GraphWalk_SegmentAppend(nextVertex);
                // Increment weight
                g_weightArray[nextVertex]++;
                // The following will increment the weight array and create blocks if necessary
                g_currentExpansion--;
                g_currentSinkVertex = nextVertex;
                break;
            }
        }
    }
    while(!foundSource); /* Traceback */
    g_tracingBack = false;
}

void GraphWalk_UpdateBlocksFromWeight(void)
{
    // Channels can handle a maximum of g_channelWidth
    // Intersections can handle a maximum of g_channelWidth * 2
    // In the future, there will be another array that will store a vertex's type
    // For now, determine based on its place in the grid, and increment accordingly
    // If we reach the max, we turn the vertex into a block
    
    for(int vertex = 0; vertex < g_vertexArraySize; vertex++)
    {
        if(g_traceArray[vertex] == VERTEX_BLOCK)
        {
            continue;
        }
        int col = vertex % g_sideLength;
        int row = vertex / g_sideLength;
        
        if(row % 2 == 0)
        {
            if(col % 2)
            {
                // Intersection
                // Check if it's become a block
                if(g_weightArray[vertex] >= g_channelWidth * 2)
                {
                    g_traceArray[vertex] = VERTEX_BLOCK;
                }
            }
            else
            {
                // Channel
                // Check if it's become a block
                if(g_weightArray[vertex] >= g_channelWidth)
                {
                    g_traceArray[vertex] = VERTEX_BLOCK;
                }
            }
        }
        else
        {
            // Only channels on odd rows
            // Check if it's become a block
            if(g_weightArray[vertex] >= g_channelWidth)
            {
                g_traceArray[vertex] = VERTEX_BLOCK;
            }
        }
    }
}

void GraphWalk_InitNetRoutes(void)
{
    // Init all net routes
    // Our route is initially completely empty
    GraphWalk_WaitLockNetsSegments();
    GraphWalk_FreeNetRoutes();
    memset((void*)&g_netRoutes, 0, sizeof(netRoutes_t));
    GraphWalk_LockNetSegments(false);
}

void GraphWalk_FreeNetRoutes(void)
{
    GraphWalk_WaitLockNetsSegments();
    free(g_netRoutes.netRouteArrayPointer);
    free(g_netRoutes.netRouteArrayPointer);
    free(g_netRoutes.segmentIdArrayPointer);
    free(g_netRoutes.segmentVertexArrayPointer);
    GraphWalk_LockNetSegments(false);
}

void GraphWalk_NewNetRoute(void)
{
    GraphWalk_WaitLockNetsSegments();
    // Increment the size of the net route array
    g_netRoutes.netRouteArraySize++;
    // Reallocate memory
    g_netRoutes.netRouteArrayPointer = (int*)realloc(g_netRoutes.netRouteArrayPointer, g_netRoutes.netRouteArraySize * sizeof(int));
    // Add the index
    g_netRoutes.netRouteArrayPointer[g_netRoutes.netRouteArraySize - 1] = g_netRoutes.segmentIdArraySize;
    GraphWalk_LockNetSegments(false);
}

void GraphWalk_NewSegment(void)
{
    GraphWalk_WaitLockNetsSegments();
    g_netRoutes.lastSafeSegment = g_netRoutes.segmentIdArraySize;
    // Increment the size of the segment ID array
    g_netRoutes.segmentIdArraySize++;
    // Reallocate the memory
    g_netRoutes.segmentIdArrayPointer = (int*)realloc(g_netRoutes.segmentIdArrayPointer, g_netRoutes.segmentIdArraySize * sizeof(int));
    // Add the index
    g_netRoutes.segmentIdArrayPointer[g_netRoutes.segmentIdArraySize - 1] = g_netRoutes.segmentVertexArraySize;
    GraphWalk_LockNetSegments(false);
}

void GraphWalk_SegmentAppend(int vertex)
{
    GraphWalk_WaitLockNetsSegments();
    // Increment the size of the segment vertex array
    g_netRoutes.segmentVertexArraySize++;
    // Reallocate the memory
    g_netRoutes.segmentVertexArrayPointer = (int*)realloc(g_netRoutes.segmentVertexArrayPointer, g_netRoutes.segmentVertexArraySize * sizeof(int));
    // Add the vertex
    g_netRoutes.segmentVertexArrayPointer[g_netRoutes.segmentVertexArraySize - 1] = vertex;
    GraphWalk_LockNetSegments(false);
}

netRoutes_t GraphWalk_GetNetRoutes(void)
{
    return g_netRoutes;
}

bool GraphWalk_IsNetUnconnected(int startIndex, int endIndex, int * netStatusArray)
{
    bool rv = false;
    
    for(int i = startIndex; i < endIndex; i++)
    {
        if(netStatusArray[i] == NET_UNCONNECTED)
        {
            rv = true;
            break;
        }
    }
    return rv;
}

bool GraphWalk_IsMaskArrayEmpty(int * maskArray, int maskArraySize)
{
    bool rv = true;
    
    // Go through the mask array and see if we need to visit anything
    for(int i = 0; i < maskArraySize; i++)
    {
        if(maskArray[i] == MASK_VISIT_NEXT)
        {
            rv = false;
            break;
        }
    }
    return rv;
}

int * GraphWalk_GetWeightArray(void)
{
    return g_weightArray;
}
