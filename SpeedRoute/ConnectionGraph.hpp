//
//  ConnectionGraph.hpp
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-25.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#ifndef ConnectionGraph_hpp
#define ConnectionGraph_hpp

#include <vector>

#include "FileParser.hpp"
#include "Types.h"

// Vertex struct
typedef struct
{
    int     id;
    int     neighbour[DIR_NUM];
} vertex_t;

typedef std::vector<std::vector<vertex_t>> vertexGrid;

class ConnectionGraph
{
public:
    ConnectionGraph(posVec placement, int numCol, int numRows, int padding);
    ~ConnectionGraph(void);
    
    vertexGrid getGrid(void);  
    graphData_t getGraphData(void);
    int getVertexIdFromPlacement(int col, int row);
    netData_t getNetVectors(netVec rawNets, posVec rawPos);
    
private:
    void generateGraph();
    
    int mPlacementWidth;
    int mPlacementHeight;
    int mSideLength;
    int mPadding;
    vertexGrid mGrid;
    posVec mPlacement;
    std::vector<int> mVertexVector;
    std::vector<int> mEdgeVector;
    std::vector<int> mNetIdVector;
    std::vector<int> mNetVertexVector;
};

#endif /* ConnectionGraph_hpp */
