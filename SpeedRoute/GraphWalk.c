//
//  GraphWalk.c
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-28.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#include "GraphWalk.h"

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