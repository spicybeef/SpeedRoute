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

typedef enum
{
    STATE_IDLE = 0,
    STATE_EXPANSION,
    STATE_TRACEBACK,
    STATE_ROUTE_SUCCESS,
    STATE_ROUTE_FAILURE
} routingState_e;
\
void GraphWalk_Test(int priority, graphData_t graph, netData_t nets);
void GraphWalk_DebugPrint(int priority, const char * format, ... );
void GraphWalk_DebugPrintGrid(int priority, char * string, int * gridArray);
void GraphWalk_DebugPrintRoutes(int priority);

void GraphWalk_Main(bool clEnable);
routingState_e GraphWalk_GetCurrentState(void);
int GraphWalk_GetCurrentNetNum(void);
int GraphWalk_GetTotalNetNum(void);
int GraphWalk_GetCurrentSourceVertex(void);
int GraphWalk_GetCurrentSinkVertex(void);
int GraphWalk_GetCurrentExpansion(void);
bool GraphWalk_IsRoutingRunning(void);
bool GraphWalk_IsTracingBack(void);
void GraphWalk_LockNetSegments(bool lock);
void GraphWalk_WaitLock(bool * lockVar);
void GraphWalk_WaitLockNetsSegments(void);

void GraphWalk_InitWalkData(graphData_t graph, netData_t nets, int channelWidth);
void GraphWalk_FreeWalkData(void);

void GraphWalk_InitMask(void);
void GraphWalk_InitTrace(void);
void GraphWalk_InitWeight(void);
void GraphWalk_InitNetStatus(void);

bool GraphWalk_RouteNet(bool openCl, int netId);
void GraphWalk_WavefrontVisit(void);
void GraphWalk_WavefrontVisit_Cl(void);
void GraphWalk_UpdateTraceAndMask(void);
void GraphWalk_TraceBack(void);

void GraphWalk_UpdateBlocksFromWeight(void);

void GraphWalk_InitNetRoutes(void);
void GraphWalk_FreeNetRoutes(void);
void GraphWalk_NewNetRoute(void);
void GraphWalk_NewSegment(void);
void GraphWalk_SegmentAppend(int vertex);
netRoutes_t GraphWalk_GetNetRoutes(void);

bool GraphWalk_IsNetUnconnected(int startIndex, int endIndex, int * netStatusArray);
bool GraphWalk_IsMaskArrayEmpty(int * maskArray, int maskArraySize);

int * GraphWalk_GetWeightArray(void);
    
#endif /* GraphWalk_h */
