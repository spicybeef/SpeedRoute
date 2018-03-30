//
//  GraphWalk.h
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-28.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#ifndef GraphWalk_h
#define GraphWalk_h

#include "Types.h"

void GraphWalk_Test(graphData_t data);

void GraphWalk_InitArrays(graphData_t data);
void GraphWalk_FreeArrays(graphData_t data);

netStruct_t * GraphWalk_InitNet(int * nodes, posStruct_t * placement, int numNodes);
void GraphWalk_FreeNet(netStruct_t * net);

void GraphWalk_RouteNet(netStruct_t * net);
    
#endif /* GraphWalk_h */
