//
//  GraphWalk.h
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-28.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#ifndef GraphWalk_h
#define GraphWalk_h

#include <stdbool.h>

#include "Types.h"

void GraphWalk_Test(graphData_t graph, netData_t nets);
void GraphWalk_DebugPrintGrid(char * string, int * gridArray);
void GraphWalk_InitWalkData(graphData_t graph, netData_t nets);
void GraphWalk_FreeWalkData(graphData_t data);
void GraphWalk_InitMask(void);
void GraphWalk_InitTrace(void);
void GraphWalk_InitWeight(void);
void GraphWalk_InitNetStatus(void);
void GraphWalk_RouteNet(int netId);
bool GraphWalk_IsNetUnconnected(int startIndex, int endIndex, int * netStatusArray);
bool GraphWalk_IsMaskArrayEmpty(int * maskArray, int maskArraySize);
    
#endif /* GraphWalk_h */
