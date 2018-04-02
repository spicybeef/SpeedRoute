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

// Module scope data

static int g_debugPriority = PRIO_HIGH;

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
// The following arrays are used to store a net's route
// Hierarchy:
// Net ID -> Segment ID -> Vertex ID
static int * g_netRouteArrayPointer;
static int g_netRouteArraySize;
static int * g_segmentIdArrayPointer;
static int g_segmentIdArraySize;
static int * g_segmentVertexArrayPointer;
static int g_segmentVertexArraySize;

// Architecture side length
static int g_sideLength;
// Architecture channel width
static int g_channelWidth;

// Routing variables
static int g_currentSourceVertex;
static int g_currentSinkVertex;

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
    
    if(priority >= g_debugPriority)
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
    
    if(priority >= g_debugPriority)
    {
        vprintf(format, args);
    }
    
    va_end(args);
}

void GraphWalk_DebugPrintGrid(int priority, char * string, int * gridArray)
{
    if(priority >= g_debugPriority)
    {
        printf("%s\n", string);
        for(int row = 0; row < g_sideLength; row++)
        {
            for(int col = 0; col < g_sideLength; col++)
            {
                {
                    if(gridArray[col + row * g_sideLength] == 0)
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
    if(priority >= g_debugPriority)
    {
        printf("Route summary:\n");
        
        printf("Net route array: ");
        for(int i = 0; i < g_netRouteArraySize; i++)
        {
            int value = g_netRouteArrayPointer[i];
            printf("%i ", value);
        }
        printf("\n");
        
        printf("Net segment ID array: ");
        for(int i = 0; i < g_segmentIdArraySize; i++)
        {
            int value = g_segmentIdArrayPointer[i];
            printf("%i ", value);
        }
        printf("\n");
        
        printf("Net segment vertex array: ");
        for(int i = 0; i < g_segmentVertexArraySize; i++)
        {
            int value = g_segmentVertexArrayPointer[i];
            printf("%i ", value);
        }
        printf("\n");
    }
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

void GraphWalk_FreeWalkData(graphData_t data)
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
    // Init mask array to MASK_NO_VISIT
    for(int i = 0; i < g_vertexArraySize; i++)
    {
        g_maskArray[i] = MASK_NO_VISIT;
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

bool GraphWalk_RouteNet(int netId)
{
    time_t t;
    // Seed random number
    srand((unsigned) time(&t));
    bool firstNetVertex = true;
    
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
        GraphWalk_DebugPrint(PRIO_LOW, "Sourcing from %d\n", g_currentSourceVertex);
        g_maskArray[g_currentSourceVertex] = MASK_VISIT;
        // This net's vertex becomes a connected source, mark it as such
        g_netStatusArray[randNetVertexIndex] = NET_CONNECTED;
        
        /* Wavefront Expansion */
        int currentExpansion = 0;
        bool sinkFound = false;
        for(;;)
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
                            if(firstNetVertex)
                            {
                                // No longer the first vertex
                                firstNetVertex = false;
                            }
                            else
                            {
                                // We've started connecting nets already, ignore unconnected ones
                                if(g_traceArray[nextVertex] == VERTEX_NET_UNCONN)
                                {
                                    continue;
                                }
                            }
                            // Stop the presses, we've found a sink
                            sinkFound = true;
                            GraphWalk_DebugPrint(PRIO_LOW, "Found sink @ %d!\n", nextVertex);
                            // We're going to trace back from here
                            g_currentSinkVertex = nextVertex;
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
                            // Trigger exit from this loop by blanking out the mask array
                            GraphWalk_InitMask();
                            break;
                        }
                        g_traceArray[nextVertex] = currentExpansion + 1;
                        g_maskArray[nextVertex] = MASK_VISIT_NEXT;
                    }
                }
            }
            
            // Print out some info
            GraphWalk_DebugPrintGrid(PRIO_LOW, "Trace Array", g_traceArray);
            GraphWalk_DebugPrintGrid(PRIO_LOW, "Mask Array", g_maskArray);
            
            if(!GraphWalk_IsMaskArrayEmpty(g_maskArray, g_vertexArraySize))
            {
                // Not empty yet. Change next visits to visits
                // This is required to synchronize the work done previously, preventing visiting things we just marked for visitation
                for(int vertex = 0; vertex < g_vertexArraySize; vertex++)
                {
                    // Check if we need to visit
                    if(g_maskArray[vertex] == MASK_VISIT_NEXT)
                    {
                        g_maskArray[vertex] = MASK_VISIT;
                    }
                }
                // Go to the next expansion!
                currentExpansion++;
                continue;
            }
            // If it's empty, check if we've found a sink
            else if(sinkFound)
            {
                // Sink has been found, route success
                // Sink is equivalent to the next expansion
                currentExpansion++;
                break;
            }
            // No sink found, route failed
            else
            {
                GraphWalk_DebugPrint(PRIO_HIGH, "Route failed for net ID: %d!\n", netId);
                return false;
            }
            
        } /* Wavefront expansion */
        
        // We've got a new segment
        GraphWalk_NewSegment();
        
        /* Traceback */
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
                    GraphWalk_DebugPrint(PRIO_LOW, "Found the original source @ %d\n", nextVertex);
                    GraphWalk_SegmentAppend(nextVertex);
                    foundSource = true;
                    break;
                }
                // Check if it's a route back
                if(g_traceArray[nextVertex] == currentExpansion - 1)
                {
                    GraphWalk_DebugPrint(PRIO_LOW, "Found a way back through %d\n", nextVertex);
                    GraphWalk_SegmentAppend(nextVertex);
                    // Increment weight
                    g_weightArray[nextVertex]++;
                    // The following will increment the weight array and create blocks if necessary
                    currentExpansion--;
                    g_currentSinkVertex = nextVertex;
                    break;
                }
            }
        }
        while(!foundSource); /* Traceback */
        
        // Change vertexes at capacity into blocks
        GraphWalk_UpdateBlocksFromWeight();
        
        GraphWalk_DebugPrint(PRIO_LOW, "Net status: ");
        for(int i = 0; i < (netVertexIndexEnd - netVertexIndexStart); i++)
        {
            GraphWalk_DebugPrint(PRIO_LOW, "%d ", g_netStatusArray[i]);
        }
        GraphWalk_DebugPrint(PRIO_LOW, "\n");
    }
    while(GraphWalk_IsNetUnconnected(netVertexIndexStart, netVertexIndexEnd, g_netStatusArray));
    
    GraphWalk_DebugPrintGrid(PRIO_NORM, "Weights after routing net ID %d", g_weightArray);
    
    // If we're here we've successfully routed
    return true;
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
    g_netRouteArrayPointer = NULL;
    g_netRouteArraySize = 0;
    g_segmentIdArrayPointer = NULL;
    g_segmentIdArraySize = 0;
    g_segmentVertexArrayPointer = NULL;
    g_segmentVertexArraySize = 0;
}

void GraphWalk_FreeNetRoutes(void)
{
    free(g_netRouteArrayPointer);
    free(g_netRouteArrayPointer);
    free(g_segmentIdArrayPointer);
    free(g_segmentVertexArrayPointer);
}

void GraphWalk_NewNetRoute(void)
{
    // Increment the size of the net route array
    g_netRouteArraySize++;
    // Reallocate memory
    g_netRouteArrayPointer = realloc(g_netRouteArrayPointer, g_netRouteArraySize * sizeof(int));
    // Add the index
    g_netRouteArrayPointer[g_netRouteArraySize - 1] = g_segmentIdArraySize;
}

void GraphWalk_NewSegment(void)
{
    // Increment the size of the segment ID array
    g_segmentIdArraySize++;
    // Reallocate the memory
    g_segmentIdArrayPointer = realloc(g_segmentIdArrayPointer, g_segmentIdArraySize * sizeof(int));
    // Add the index
    g_segmentIdArrayPointer[g_segmentIdArraySize - 1] = g_segmentVertexArraySize;
}

void GraphWalk_SegmentAppend(int vertex)
{
    // Increment the size of the segment vertex array
    g_segmentVertexArraySize++;
    // Reallocate the memory
    g_segmentVertexArrayPointer = realloc(g_segmentVertexArrayPointer, g_segmentVertexArraySize * sizeof(int));
    // Add the vertex
    g_segmentVertexArrayPointer[g_segmentVertexArraySize - 1] = vertex;
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
