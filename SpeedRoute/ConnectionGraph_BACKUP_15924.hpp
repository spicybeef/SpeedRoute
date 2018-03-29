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

typedef vertexGrid std::vector<std::vector<vertex_t>>;

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
    vertexGrid getGrid(void);
    std::vector<int> getVertexVector(void);
    std::vector<int> getEdgeVector(void);
    
private:
    int                                 mSideLength;
    vertexGrid                          mGrid;
    std::vector<int>                    mVertexVector;
    std::vector<int>                    mEdgeVector;
    std::vector<int>                    mCongestionVector;
<<<<<<< HEAD
    std::vector<int>                    mCountVector;
=======
>>>>>>> eed9d003b9d8771e8ec855cb75da655da475b15b
};

#endif /* ConnectionGraph_hpp */
