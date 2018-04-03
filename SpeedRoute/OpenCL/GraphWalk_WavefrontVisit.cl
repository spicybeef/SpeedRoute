//
//  GraphWalk_WavefrontVisit.cl
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-04-02.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#define VERTEX_NONETYPE         -1
#define VERTEX_BLOCK            -2
#define VERTEX_NET_UNCONN       -3
#define VERTEX_NET_CONN         -4

#define MASK_NO_VISIT           0
#define MASK_VISIT              1
#define MASK_VISIT_NEXT         2
#define MASK_VISITED            3

kernel void GraphWalk_WavefrontVisit(
                                     global const int * vertexArray,
                                     global const int * edgeArray,
                                     global int * maskArray,
                                     global int * traceArray,
                                     global int * sinkFound,
                                     global int * sinkVertex,
                                     int firstNetVertex,
                                     int vertexArraySize
                                     )
{
    int vertex = get_global_id(0);
    int nextVertex;
    int start, end;
    
    // Check if we need to visit
    if(maskArray[vertex] == MASK_VISIT)
    {
        // We do! Turn it into a visited box
        maskArray[vertex] = MASK_VISITED;
        // Index of vertex array points to entry in edge array
        // Index + 1 of vertex array points to the first node of the next vertex's edges
        // Determine the start and end of the net vertex array to index for this net ID
        start = vertexArray[vertex];
        // Check if we're at the end of the array
        if ((vertex + 1) < (vertexArraySize))
        {
            // We're still within bounds, end is next index
            end = vertexArray[vertex + 1];
        }
        else
        {
            // We've reached the end, the size is the end
            end = vertexArraySize;
        }
        
        // Now go through the vertex's edges
        for(int edgeIndex = start; edgeIndex < end; edgeIndex++)
        {
            nextVertex = edgeArray[edgeIndex];
            
            if(traceArray[nextVertex] == VERTEX_BLOCK)
            {
                // Can't go here, go to next edge
                continue;
            }
            // Check if it's been visited
            if(maskArray[nextVertex] == MASK_VISITED)
            {
                // Can't go back, go to next edge
                continue;
            }
            // Check if it's a valid connection
            if(traceArray[nextVertex] == VERTEX_NET_UNCONN || traceArray[nextVertex] == VERTEX_NET_CONN)
            {
                // OK, so if it's the first net vertex, we can sink to anything.
                // Subsequent sinks MUST be already connected to avoid unconnected graphs.
                // We run the risk of some ugly connections, but at least they'll lead to fully connected graphs.
                if(firstNetVertex)
                {
                    // We've started connecting nets already, ignore unconnected ones
                    if(traceArray[nextVertex] == VERTEX_NET_UNCONN)
                    {
                        continue;
                    }
                }
                // Stop the presses, we've found a sink
                *sinkFound = 1;
                // We're going to trace back from here
                *sinkVertex = nextVertex;
                break;
            }
            maskArray[nextVertex] = MASK_VISIT_NEXT;
        }
    }
}
