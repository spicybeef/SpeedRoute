//
//  GraphWalk.c
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-28.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#include "GraphWalk.h"

static int * traceArray;
static int * maskArray;
static int * weightArray;

void GraphWalk_Test(graphData_t data)
{
    int i, arraySize, value;
    
    arraySize = data.vertexArraySize;
    printf("Vertex array: ");
    for(i = 0; i < arraySize; i++)
    {
        value = data.vertexArrayPointer[i];
        printf("%i ", value);
    }
    printf("\n");
    
    arraySize = data.edgeArraySize;
    printf("Edge array: ");
    for(i = 0; i < arraySize; i++)
    {
        value = data.edgeArrayPointer[i];
        printf("%i ", value);
    }
    printf("\n");
}

void GraphWalk_InitArrays(graphData_t data)
{
    traceArray = malloc(data.edgeArraySize * sizeof(int));
    maskArray = malloc(data.edgeArraySize * sizeof(int));
    weightArray = malloc(data.edgeArraySize * sizeof(int));
}

void GraphWalk_FreeArrays(graphData_t data)
{
    free(traceArray);
    free(maskArray);
    free(weightArray);
}

netStruct_t * GraphWalk_InitNet(int * nodes, posStruct_t * placement, int numNodes)
{
    // Allocate memory for a net struct
    netStruct_t * newNet = malloc(sizeof(netStruct_t));
    // Allocate memory for the node positions
    newNet->pos = malloc(numNodes * sizeof(posStruct_t));
    // Fill in the number of nodes
    newNet->numNodes = numNodes;
    
    for(int i = 0; i < numNodes; i++)
    {
        newNet->pos[i] = placement[i];
    }
    
    return newNet;
}

void GraphWalk_FreeNet(netStruct_t * net)
{
    // Free the net's node positions
    free(net->pos);
    // Free the net itself
    free(net);
}

void GraphWalk_RouteNet(netStruct_t * net)
{
}

// Need the following (mostly obtained from ConnectionGraph)
// TODO: Create a C API to return pointers to these things from the ConnectionGraph class

// vertexArray, vertexArray size
// * each index points to its first edge in edgeArray
// * total number of edges a vertex it has is vertexArray[it+1] -
//   vertexArray[it]
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
// maskArray, maskArray is the same size as edgeArray
// * determines what to do with a vertex
// * initialized to 0
// * when maskArray[it] = 0 don't visit
// * when maskArray[it] = 1 visit for expansion
// * when maskArray[it] > 1 already visited, so don't visit
//
// weightArray, weightArray is the same size as edgeArray
// * keeps track of edge weights, eventually routing is impossible when edge
//   weight = channel width
// * initialized to 0 at beginning of routing
