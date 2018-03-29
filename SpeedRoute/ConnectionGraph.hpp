//
//  ConnectionGraph.hpp
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-25.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#ifndef ConnectionGraph_hpp
#define ConnectionGraph_hpp

#ifdef __cplusplus
#include <vector>
#endif /* _cplusplus */

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

#ifdef __cplusplus
typedef std::vector<std::vector<vertex_t>> vertexGrid;

class ConnectionGraph
{
public:
    ConnectionGraph(void);
    ~ConnectionGraph(void);
    
    void generateGraph(int sideLen);
    
    vertexGrid getGrid(void);
    std::vector<int> getVertexVector(void);
    std::vector<int> getEdgeVector(void);
    int * getVertexArrayPointer(void);
    int * getEdgeArrayPointer(void);
    int getVertexArraySize(void);
    int getEdgeArraySize(void);
    
private:
    int                                 mSideLength;
    vertexGrid                          mGrid;
    std::vector<int>                    mVertexVector;
    std::vector<int>                    mEdgeVector;
    std::vector<int>                    mCongestionVector;
    std::vector<int>                    mCountVector;
};
#else /* __cplusplus */
typedef struct ConnectionGraph ConnectionGraph;
#endif /* __cplusplus */

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
int * ConnectionGraph_GetVertexArrayPointer(ConnectionGraph * cgPointer);
int * ConnectionGraph_GetEdgeArrayPointer(ConnectionGraph * cgPointer);
int ConnectionGraph_GetVertexArraySize(ConnectionGraph * cgPointer);
int ConnectionGraph_GetEdgeArraySize(ConnectionGraph * cgPointer);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ConnectionGraph_hpp */
