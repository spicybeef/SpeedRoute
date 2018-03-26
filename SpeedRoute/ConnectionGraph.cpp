//
//  ConnectionGraph.cpp
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-25.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#include "ConnectionGraph.hpp"

ConnectionGraph::ConnectionGraph(void)
{

}

ConnectionGraph::~ConnectionGraph(void)
{
    
}

void ConnectionGraph::generateGraph(int sideLen)
{
    unsigned int row, col;
    std::vector<vertex_t> tempCol;
    vertex_t tempVertex;
    vertex_t *tempVertexPointer;
    
    mSideLength = sideLen;
    
    // Populate the grid
    for(col = 0; col < mSideLength; col++)
    {
        // Clear out the temporary column
        tempCol.clear();
        // Fill up a row
        for(row = 0; row < mSideLength; row++)
        {
            tempVertex.id = row + (col * row);
            
            tempVertex.neighbour[DIR_NORTH] = -1;
            tempVertex.neighbour[DIR_EAST] = -1;
            tempVertex.neighbour[DIR_SOUTH] = -1;
            tempVertex.neighbour[DIR_WEST] = -1;
            
            tempCol.push_back(tempVertex);
        }
        // Push back the column onto the grid
        mGrid.push_back(tempCol);
    }
    
    // Make the neighbour connections
    for(col = 0; col < mSideLength; col++)
    {
        for(row = 0; row < mSideLength; row++)
        {
            tempVertexPointer = &mGrid[col][row];
            // Link northern neighbours
            if(row > 0)
            {
                tempVertexPointer->neighbour[DIR_NORTH] = mGrid[col][row - 1].id;
            }
            // Link eastern neighbours
            if(col < mSideLength - 1)
            {
                tempVertexPointer->neighbour[DIR_EAST] = mGrid[col + 1][row].id;
            }
            // Link southern neighbours
            if(row < mSideLength - 1)
            {
                tempVertexPointer->neighbour[DIR_SOUTH] = mGrid[col][row + 1].id;
            }
            // Link western neighours
            if(col > 0)
            {
                tempVertexPointer->neighbour[DIR_WEST] = mGrid[col - 1][row].id;
            }
        }
    }
}
