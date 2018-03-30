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
    ConnectionGraph(int numCol, int numRows, int padding);
    ~ConnectionGraph(void);
    
    vertexGrid getGrid(void);
    std::vector<int> getVertexVector(void);
    std::vector<int> getEdgeVector(void);
    int * getVertexArrayPointer(void);
    int * getEdgeArrayPointer(void);
    int getVertexArraySize(void);
    int getEdgeArraySize(void);
    graphData_t getGraphData(void);
    int getVertexIdFromPlacement(int col, int row);
    
private:
    int                                 mPlacementWidth;
    int                                 mPlacementHeight;
    int                                 mSideLength;
    int                                 mPadding;
    vertexGrid                          mGrid;
    std::vector<int>                    mVertexVector;
    std::vector<int>                    mEdgeVector;
    std::vector<int>                    mCongestionVector;
    std::vector<int>                    mCountVector;
    
    void generateGraph();
};

#endif /* ConnectionGraph_hpp */
