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

// Direction enum
typedef enum
{
    DIR_NORTH = 0,
    DIR_EAST,
    DIR_SOUTH,
    DIR_WEST,
    DIR_NUM
} dir_e;

// Vertex struct
typedef struct
{
    int     id;
    int     neighbour[DIR_NUM];
} vertex_t;

class ConnectionGraph
{
public:
    ConnectionGraph(void);
    ~ConnectionGraph(void);
    
    void generateGraph(int sideLen);
    
private:
    int                                 mSideLength;
    std::vector<std::vector<vertex_t>>  mGrid;
    std::vector<int>                    mVertexVector;
    std::vector<int>                    mEdgeVector;
};

#endif /* ConnectionGraph_hpp */
