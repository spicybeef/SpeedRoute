//
//  GraphWalk.c
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-28.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "GraphWalk.h"

static int * g_traceArray;
static int * g_maskArray;
static int * g_weightArray;
static int * g_netStatusArray;
static int g_vertexArraySize;
static int g_edgeArraySize;

#define VERTEX_NONETYPE         0
#define VERTEX_BLOCK            1
#define VERTEX_NET_SINK_UNCONN  2
#define VERTEX_NET_SINK_CONN    3
#define VERTEX_NET_SOURCE       4

#define MASK_NO_VISIT           0
#define MASK_VISIT              1
#define MASK_VISIT_NEXT         2
#define MASK_VISITED            3

#define NET_NONETYPE            0
#define NET_CONNECTED           1
#define NET_UNCONNECTED         2

void GraphWalk_Test(graphData_t graph, netData_t nets)
{
    int arraySize, value;
    
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

void GraphWalk_DebugPrintGrid(char * string, int * blockage, int * gridArray, int sideLength)
{
    printf("%s:\n", string);
    for(int row = 0; row < sideLength; row++)
    {
        for(int col = 0; col < sideLength; col++)
        {
//            if(blockage[col + row * sideLength] >= VERTEX_BLOCK)
//            {
//                printf(" X ");
//            }
//            else
            {
                if(gridArray[col + row * sideLength] == 0)
                {
                    printf(" . ");
                }
                else
                {
                    printf("%2i ", gridArray[col + row * sideLength]);
                }
            }
        }
        printf("\n");
    }
}

void GraphWalk_InitArrays(graphData_t data)
{
    g_vertexArraySize = data.vertexArraySize;
    g_edgeArraySize = data.edgeArraySize;
    
    g_maskArray = malloc(data.vertexArraySize * sizeof(int));
    g_netStatusArray = malloc(data.vertexArraySize * sizeof(int));
    
    g_traceArray = malloc(data.edgeArraySize * sizeof(int));
    g_weightArray = malloc(data.edgeArraySize * sizeof(int));
}

void GraphWalk_FreeArrays(graphData_t data)
{
    free(g_traceArray);
    free(g_maskArray);
    free(g_weightArray);
    free(g_netStatusArray);
}

void GraphWalk_InitMask(void)
{
    // Init mask array to MASK_NO_VISIT
    memset(g_maskArray, MASK_NO_VISIT, g_vertexArraySize * sizeof(int));
}

void GraphWalk_InitTrace(void)
{
    // Init trace array to -1
    memset(g_traceArray, -1, g_edgeArraySize * sizeof(int));
}

void GraphWalk_InitWeight(void)
{
    // Init weight array to 0
    memset(g_weightArray, 0, g_vertexArraySize * sizeof(int));
}

void GraphWalk_InitNetStatus(netData_t nets)
{
    // Init net status array to NET_NONETYPE
    // We'll have to visit and connect the same about of connects as in netVertexArraySize, so use that for init
    memset(g_netStatusArray, NET_UNCONNECTED, nets.netVertexArraySize * sizeof(int));
}

void GraphWalk_RouteNet(graphData_t graph, netData_t nets, int netId)
{
    time_t t;
    // Seed random number
    srand((unsigned) time(&t));
    bool firstNetVertex = true;
    
    int netVertexIndexStart, netVertexIndexEnd, randNetVertexIndex, currentSourceVertex, currentSinkVertex;
    
    // Index of net ID array points to the first node of a net
    // Index + 1 of net ID array points to the first node of the next net
    // Determine the start and end of the net vertex array to index for this net ID
    netVertexIndexStart = nets.netIdArrayPointer[netId];
    // Check if we're at the end of the array
    if ((netId + 1) < (nets.netIdArraySize))
    {
        // We're still within bounds, end is next index
        netVertexIndexEnd = nets.netIdArrayPointer[netId + 1];
    }
    else
    {
        // We've reached the end, the size is the end
        netVertexIndexEnd = nets.netVertexArraySize;
    }
    printf("Net ID %d start and end index: %d %d\n", netId, netVertexIndexStart, netVertexIndexEnd);
    
    // Update blockage array with current net vertexes
    for(int vertexIndex = netVertexIndexStart; vertexIndex < netVertexIndexEnd; vertexIndex++)
    {
        graph.blockageArrayPointer[nets.netVertexArrayPointer[vertexIndex]] = VERTEX_NET_SINK_UNCONN;
    }
    
    // Init the mask
    GraphWalk_InitMask();
    // Init the trace
    GraphWalk_InitTrace();
    
    // Route until we run out of vertexes to route for this net
    do
    {
        // Pick a random vertex index to start routing from
        randNetVertexIndex = (rand() % (netVertexIndexEnd - netVertexIndexStart)) + netVertexIndexStart;
        currentSourceVertex = nets.netVertexArrayPointer[randNetVertexIndex];
        // Check if this one has been connected already
        if(g_netStatusArray[randNetVertexIndex] == NET_CONNECTED)
        {
            continue;
        }
        g_maskArray[currentSourceVertex] = MASK_VISIT;
        // This net's vertex becomes a connected source, mark it as such
        g_netStatusArray[randNetVertexIndex] = NET_CONNECTED;
        // This net's vertex is the seed of the trace, so mark it as 0
        g_traceArray[currentSourceVertex] = 0;
        
        GraphWalk_DebugPrintGrid("Mask Array", graph.blockageArrayPointer, g_maskArray, graph.sideLength);
        
        /* Wavefront Expansion */
        int currentExpansion = 0;
        while(!GraphWalk_IsMaskArrayEmpty(g_maskArray, graph.vertexArraySize))
        {
            // Visit masked areas
            for(int vertex = 0; vertex < graph.vertexArraySize; vertex++)
            {
                // Check if we need to visit
                if(g_maskArray[vertex] == MASK_VISIT)
                {
                    // We do! Turn it into a visited box
                    g_maskArray[vertex] = MASK_VISITED;
                    // Index of vertex array points to entry in edge array
                    // Index + 1 of vertex array points to the first node of the next vertex's edges
                    // Determine the start and end of the net vertex array to index for this net ID
                    int start = graph.vertexArrayPointer[vertex];
                    int end;
                    // Check if we're at the end of the array
                    if ((vertex + 1) < (graph.vertexArraySize))
                    {
                        // We're still within bounds, end is next index
                        end = graph.vertexArrayPointer[vertex + 1];
                    }
                    else
                    {
                        // We've reached the end, the size is the end
                        end = graph.vertexArraySize;
                    }
                    
                    // Now go through the vertex's edges
                    for(int edgeIndex = start; edgeIndex < end; edgeIndex++)
                    {
                        int nextVertex = graph.edgeArrayPointer[edgeIndex];
                        printf("Visiting %d\n", nextVertex);
                        // Check if it's blocked
                        if(graph.blockageArrayPointer[nextVertex] == VERTEX_BLOCK)
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
                        if(graph.blockageArrayPointer[nextVertex] == VERTEX_NET_SINK_UNCONN || graph.blockageArrayPointer[nextVertex] == VERTEX_NET_SINK_CONN)
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
                                if(graph.blockageArrayPointer[nextVertex] == VERTEX_NET_SINK_UNCONN)
                                {
                                    continue;
                                }
                            }
                            // Stop the presses, we've found a sink
                            printf("Found sink @ %d!\n", nextVertex);
                            // We're going to trace back from here
                            currentSinkVertex = nextVertex;
                            // Update blockage array
                            graph.blockageArrayPointer[currentSinkVertex] = VERTEX_NET_SINK_CONN;
                            graph.blockageArrayPointer[currentSourceVertex] = VERTEX_NET_SINK_CONN;
                            // Trigger exit from this loop by blanking out the mask array
                            GraphWalk_InitMask();
                            break;
                        }
                        g_traceArray[nextVertex] = currentExpansion + 1;
                        g_maskArray[nextVertex] = MASK_VISIT_NEXT;
                    }
                }
            }
            
            // Change next visits to visits
            // This is required to synchronize the work done previously, preventing visiting things we just marked for visitation
            for(int vertex = 0; vertex < graph.vertexArraySize; vertex++)
            {
                // Check if we need to visit
                if(g_maskArray[vertex] == MASK_VISIT_NEXT)
                {
                    g_maskArray[vertex] = MASK_VISIT;
                }
            }
            
            // Print out some info
            GraphWalk_DebugPrintGrid("Trace Array", graph.blockageArrayPointer, g_traceArray, graph.sideLength);
            GraphWalk_DebugPrintGrid("Mask Array", graph.blockageArrayPointer, g_maskArray, graph.sideLength);
            
            // Go to the next expansion!
            currentExpansion++;
        } /* Wavefront expansion */
        
        /* Traceback */
        printf("Tracing back from %d\n", currentSinkVertex);
        do
        {
            // Determine the start and end of the net vertex array to index for this net ID
            int start = graph.vertexArrayPointer[currentSinkVertex];
            int end;
            // Check if we're at the end of the array
            if ((currentSinkVertex + 1) < (graph.vertexArraySize))
            {
                // We're still within bounds, end is next index
                end = graph.vertexArrayPointer[currentSinkVertex + 1];
            }
            else
            {
                // We've reached the end, the size is the end
                end = graph.vertexArraySize;
            }
            // Now go through the vertex's edges
            for(int edgeIndex = start; edgeIndex < end; edgeIndex++)
            {
                int nextVertex = graph.edgeArrayPointer[edgeIndex];
                if(g_traceArray[nextVertex] == currentExpansion - 1)
                {
                    printf("Found a way back through %d\n", nextVertex);
                    currentExpansion--;
                    currentSinkVertex = nextVertex;
                    break;
                }
            }
        }
        while(currentSinkVertex != currentSourceVertex); /* Traceback */
    }
    while(GraphWalk_IsNetUnconnected(netVertexIndexStart, netVertexIndexEnd, g_netStatusArray));
}

bool GraphWalk_IsNetUnconnected(int startIndex, int endIndex, int * netStatusArray)
{
    bool rv = true;
    
    for(int i = startIndex; i < endIndex; i++)
    {
        if(netStatusArray[i] == NET_UNCONNECTED)
        {
            rv = false;
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
        if(maskArray[i] == MASK_VISIT)
        {
            rv = false;
            break;
        }
    }
    return rv;
}

// Need the following (mostly obtained from ConnectionGraph)
// TODO: Create a C API to return pointers to these things from the ConnectionGraph class

// vertexArray, vertexArray size
// * each index points to its first edge in edgeArray
// * total number of edges a vertex it has is vertexArray[it+1] -
//   vertexArray[it]
//
// maskArray, maskArray is the same size as vertexArray
// * determines what to do with a vertex
// * initialized to 0
// * when maskArray[it] = 0 don't visit
// * when maskArray[it] = 1 visit for expansion
// * when maskArray[it] > 1 already visited, so don't visit
//
// edgeArray, edgeArray size
// * a sequential list of edges for every vertex
// * indexing this array requires using vertexArray
//
// traceArray, traceArray is the same size as edgeArray
// * keeps track of the paths taken through the graph's edges
// * graphs are bidirectional, so when a path forward is found, marking the path
//   back allows one to determine where they came from
// * initialized to 0
//
// weightArray, weightArray is the same size as edgeArray
// * keeps track of edge weights, eventually routing is impossible when edge
//   weight = channel width
// * initialized to 0 at beginning of routing
