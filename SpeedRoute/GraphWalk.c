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

static int * traceArray;
static int * maskArray;
static int * weightArray;
static int vertexArraySize;
static int edgeArraySize;

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

void GraphWalk_InitArrays(graphData_t data)
{
    vertexArraySize = data.vertexArraySize;
    edgeArraySize = data.edgeArraySize;
    
    maskArray = malloc(data.vertexArraySize * sizeof(int));
    
    traceArray = malloc(data.edgeArraySize * sizeof(int));
    weightArray = malloc(data.edgeArraySize * sizeof(int));
}

void GraphWalk_FreeArrays(graphData_t data)
{
    free(traceArray);
    free(maskArray);
    free(weightArray);
}

void GraphWalk_InitRoute(void)
{
    // Init mask array to 0
    memset(maskArray, 0, vertexArraySize * sizeof(int));
    // Init trace array to 0
    memset(traceArray, 0, edgeArraySize * sizeof(int));
    // Init weight array to 0
    memset(weightArray, 0, edgeArraySize * sizeof(int));
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
    
    // Pick a node to route at random
    int nodeToRoute = rand() % (end - start);
    maskArray[nodeToRoute] = 1;
    
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
