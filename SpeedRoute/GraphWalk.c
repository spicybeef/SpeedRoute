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

// Module scope data

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
static int * g_netIdArrayPointer;
static int * g_netVertexArrayPointer;
static int g_netIdArraySize;
static int g_netVertexArraySize;

static int g_sideLength;

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

void GraphWalk_DebugPrintGrid(char * string, int * gridArray)
{
    printf("%s:\n", string);
    for(int row = 0; row < g_sideLength; row++)
    {
        for(int col = 0; col < g_sideLength; col++)
        {
            {
                if(gridArray[col + row * g_sideLength] == 0)
                {
                    printf(" . ");
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

void GraphWalk_InitWalkData(graphData_t graph, netData_t nets)
{
    g_vertexArray = graph.vertexArrayPointer;
    g_edgeArray = graph.edgeArrayPointer;
    g_vertexArraySize = graph.vertexArraySize;
    g_edgeArraySize = graph.edgeArraySize;
    
    g_netIdArrayPointer = nets.netIdArrayPointer;
    g_netVertexArrayPointer = nets.netVertexArrayPointer;
    g_netIdArraySize = nets.netIdArraySize;
    g_netVertexArraySize = nets.netVertexArraySize;
    
    g_traceArray = malloc(graph.edgeArraySize * sizeof(int));
    g_maskArray = malloc(graph.vertexArraySize * sizeof(int));
    g_weightArray = malloc(graph.edgeArraySize * sizeof(int));
    g_netStatusArray = malloc(nets.netVertexArraySize * sizeof(int));
    
    g_sideLength = graph.sideLength;
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
        // Ignore unconnected and connected nets
        if(g_traceArray[i] == VERTEX_NET_UNCONN || g_traceArray[i] == VERTEX_NET_CONN)
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
    for(int i = 0; i < g_netVertexArraySize; i++)
    {
        g_netStatusArray[i] = NET_UNCONNECTED;
    }
}

void GraphWalk_RouteNet(int netId)
{
    time_t t;
    // Seed random number
    srand((unsigned) time(&t));
    bool firstNetVertex = true;
    
    int netVertexIndexStart, netVertexIndexEnd, randNetVertexIndex, currentSourceVertex, currentSinkVertex;
    
    // Index of net ID array points to the first node of a net
    // Index + 1 of net ID array points to the first node of the next net
    // Determine the start and end of the net vertex array to index for this net ID
    netVertexIndexStart = g_netIdArrayPointer[netId];
    // Check if we're at the end of the array
    if ((netId + 1) < (g_netIdArraySize))
    {
        // We're still within bounds, end is next index
        netVertexIndexEnd = g_netIdArrayPointer[netId + 1];
    }
    else
    {
        // We've reached the end, the size is the end
        netVertexIndexEnd = g_netVertexArraySize;
    }
    printf("Net ID %d start and end index: %d %d\n", netId, netVertexIndexStart, netVertexIndexEnd);
    
    // Update trace array with current net vertexes
    for(int vertexIndex = netVertexIndexStart; vertexIndex < netVertexIndexEnd; vertexIndex++)
    {
        g_traceArray[g_netVertexArrayPointer[vertexIndex]] = VERTEX_NET_UNCONN;
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
        currentSourceVertex = g_netVertexArrayPointer[randNetVertexIndex];
        // Check if this one has been connected already
        if(g_netStatusArray[randNetVertexIndex] == NET_CONNECTED)
        {
            continue;
        }
        printf("Sourcing from %d\n", currentSourceVertex);
        g_maskArray[currentSourceVertex] = MASK_VISIT;
        // This net's vertex becomes a connected source, mark it as such
        g_netStatusArray[randNetVertexIndex] = NET_CONNECTED;
        
        /* Wavefront Expansion */
        int currentExpansion = 0;
        while(!GraphWalk_IsMaskArrayEmpty(g_maskArray, g_vertexArraySize))
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
                        printf("Visiting %d\n", nextVertex);
                        // Check if it's blocked
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
                            printf("Found sink @ %d!\n", nextVertex);
                            // We're going to trace back from here
                            currentSinkVertex = nextVertex;
                            // Update blockage array
                            g_traceArray[currentSinkVertex] = VERTEX_NET_CONN;
                            g_traceArray[currentSourceVertex] = VERTEX_NET_CONN;
                            // Our sink is now connected, mark it as such
                            for(int vertexIndex = netVertexIndexStart; vertexIndex < netVertexIndexEnd; vertexIndex++)
                            {
                                if(g_netVertexArrayPointer[vertexIndex] == currentSinkVertex)
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
            
            // Change next visits to visits
            // This is required to synchronize the work done previously, preventing visiting things we just marked for visitation
            for(int vertex = 0; vertex < g_vertexArraySize; vertex++)
            {
                // Check if we need to visit
                if(g_maskArray[vertex] == MASK_VISIT_NEXT)
                {
                    g_maskArray[vertex] = MASK_VISIT;
                }
            }
            
            // Print out some info
            GraphWalk_DebugPrintGrid("Trace Array", g_traceArray);
            GraphWalk_DebugPrintGrid("Mask Array", g_maskArray);
            
            // Go to the next expansion!
            currentExpansion++;
        } /* Wavefront expansion */
        
        /* Traceback */
        printf("Tracing back from %d\n", currentSinkVertex);
        bool foundSource = false;
        do
        {
            // Determine the start and end of the net vertex array to index for this net ID
            int start = g_vertexArray[currentSinkVertex];
            int end;
            // Check if we're at the end of the array
            if ((currentSinkVertex + 1) < (g_vertexArraySize))
            {
                // We're still within bounds, end is next index
                end = g_vertexArray[currentSinkVertex + 1];
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
                if(nextVertex == currentSourceVertex)
                {
                    printf("Found the original source @ %d\n", nextVertex);
                    foundSource = true;
                    break;
                }
                // Check if it's a route back
                if(g_traceArray[nextVertex] == currentExpansion - 1)
                {
                    printf("Found a way back through %d\n", nextVertex);
                    g_weightArray[nextVertex]++;
                    currentExpansion--;
                    currentSinkVertex = nextVertex;
                    break;
                }
            }
        }
        while(!foundSource); /* Traceback */
    }
    while(GraphWalk_IsNetUnconnected(netVertexIndexStart, netVertexIndexEnd, g_netStatusArray));
    
    GraphWalk_DebugPrintGrid("Final weights:", g_weightArray);
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
        if(maskArray[i] == MASK_VISIT)
        {
            rv = false;
            break;
        }
    }
    return rv;
}
