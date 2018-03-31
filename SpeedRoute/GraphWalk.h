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

void GraphWalk_Test(graphData_t graph, netData_t nets);
void GraphWalk_InitArrays(graphData_t data);
void GraphWalk_FreeArrays(graphData_t data);
void GraphWalk_InitRoute(void);
void GraphWalk_RouteNet(graphData_t graph, netData_t nets, int netId);
    
#endif /* GraphWalk_h */
