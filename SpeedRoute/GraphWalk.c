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
static int g_vertexArraySize;
static int g_edgeArraySize;

#define VERTEX_BLOCK            1
#define VERTEX_NET_UNROUTED     2
#define VERTEX_NET_ROUTED       3

#define MASK_NO_VISIT           0
#define MASK_VISIT              1
#define MASK_VISIT_NEXT         2
#define MASK_VISITED            3

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

void GraphWalk_DebugPrintGrid(int * blockage, int * gridArray, int sideLength)
{
    for(int row = 0; row < sideLength; row++)
    {
        for(int col = 0; col < sideLength; col++)
        {
            if(blockage[col + row * sideLength] >= VERTEX_BLOCK)
            {
                printf("X ");
            }
            else
            {
                if(gridArray[col + row * sideLength] == 0)
                {
                    printf(". ");
                }
                else
                {
                    printf("%i ", gridArray[col + row * sideLength]);
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
    
    g_traceArray = malloc(data.edgeArraySize * sizeof(int));
    g_weightArray = malloc(data.edgeArraySize * sizeof(int));
}

void GraphWalk_FreeArrays(graphData_t data)
{
    free(g_traceArray);
    free(g_maskArray);
    free(g_weightArray);
}

void GraphWalk_InitRoute(void)
{
    // Init mask array to MASK_NO_VISIT
    memset(g_maskArray, MASK_NO_VISIT, g_vertexArraySize * sizeof(int));
    // Init trace array to 0
    memset(g_traceArray, 0, g_edgeArraySize * sizeof(int));
    // Init weight array to 0
    memset(g_weightArray, 0, g_edgeArraySize * sizeof(int));
}

void GraphWalk_RouteNet(graphData_t graph, netData_t nets, int netId)
{
    time_t t;
    // Seed random number
    srand((unsigned) time(&t));
    
    GraphWalk_InitRoute();
    
    // Index of net ID array points to the first node of a net
    // Index + 1 of net ID array points to the first node of the next net
    // Determine the start and end of the net vertex array to index for this net ID
    int start = nets.netIdArrayPointer[netId];
    int end;
    // Check if we're at the end of the array
    if ((netId + 1) < (nets.netIdArraySize))
    {
        // We're still within bounds, end is next index
        end = nets.netIdArrayPointer[netId + 1];
    }
    else
    {
        // We've reached the end, the size is the end
        end = nets.netVertexArraySize;
    }
    
    printf("Net ID %d start and end index: %d %d\n", netId, start, end);
    
    // Update blockage array with current net vertexes
    for(int vertexIndex = start; vertexIndex < end; vertexIndex++)
    {
        graph.blockageArrayPointer[nets.netVertexArrayPointer[vertexIndex]] = VERTEX_NET_UNROUTED;
    }
    
    // Pick a random vertex index to start routing from
//    int randVertexIndex = (rand() % (end - start)) + start;
    int randVertexIndex = 0;
    g_maskArray[nets.netVertexArrayPointer[randVertexIndex]] = MASK_VISIT;
    graph.blockageArrayPointer[nets.netVertexArrayPointer[randVertexIndex]] = VERTEX_NET_ROUTED;
    
    printf("Mask Array:\n");
    GraphWalk_DebugPrintGrid(graph.blockageArrayPointer, g_maskArray, graph.sideLength);

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
                    g_traceArray[nextVertex] = currentExpansion + 1;
                    g_maskArray[nextVertex] = MASK_VISIT_NEXT;
                }
            }
        }
        
        printf("Trace Array:\n");
        GraphWalk_DebugPrintGrid(graph.blockageArrayPointer, g_traceArray, graph.sideLength);
        printf("Mask Array:\n");
        GraphWalk_DebugPrintGrid(graph.blockageArrayPointer, g_maskArray, graph.sideLength);
        
        // Change next visits to visits
        for(int vertex = 0; vertex < graph.vertexArraySize; vertex++)
        {
            // Check if we need to visit
            if(g_maskArray[vertex] == MASK_VISIT_NEXT)
            {
                g_maskArray[vertex] = MASK_VISIT;
            }
        }
        
        printf("Trace Array:\n");
        GraphWalk_DebugPrintGrid(graph.blockageArrayPointer, g_traceArray, graph.sideLength);
        printf("Mask Array:\n");
        GraphWalk_DebugPrintGrid(graph.blockageArrayPointer, g_maskArray, graph.sideLength);
        
        // Go to the next expansion!
        currentExpansion++;
    }
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
