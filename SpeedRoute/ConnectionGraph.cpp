//
//  ConnectionGraph.cpp
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-25.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#include "ConnectionGraph.hpp"

ConnectionGraph::ConnectionGraph(int numCol, int numRows, int padding)
{
    mPlacementWidth = numCol;
    mPlacementHeight = numRows;
    mPadding = padding;
    
    generateGraph();
}

ConnectionGraph::~ConnectionGraph(void)
{
    
}

void ConnectionGraph::generateGraph(void)
{
    unsigned int row, col, dir, i;
    std::vector<vertex_t> tempCol;
    vertex_t tempVertex;
    vertex_t *tempVertexPointer;

    // Generate the graph based on the passed in placement length
    if(mPlacementHeight > mPlacementWidth)
    {
        mSideLength = (2 * mPlacementHeight) + 1 + (mPadding * 2);
    }
    else
    {
        mSideLength = (2 * mPlacementWidth) + 1 + (mPadding * 2);
    }
    
    // Populate the grid
    for(col = 0; col < mSideLength; col++)
    {
        // Clear out the temporary column
        tempCol.clear();
        // Fill up a row
        for(row = 0; row < mSideLength; row++)
        {
            tempVertex.id = row + (col * mSideLength);
            
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
    
    // Now create the vectors that will form the C arrays
    for(col = 0; col < mSideLength; col++)
    {
        for(row = 0; row < mSideLength; row++)
        {
            // Push back the current size of the edge vector, this determines the index to use in it
            mVertexVector.push_back(static_cast<int>(mEdgeVector.size()));
            for(dir = DIR_NORTH; dir < DIR_NUM; dir++)
            {
                // For every neighbour, push back a corresponding edge
                if(mGrid[col][row].neighbour[dir] != -1)
                {
                    mEdgeVector.push_back(mGrid[col][row].neighbour[dir]);
                }
            }
        }
    }
    
    // Initialize the weight vector
    for(i = 0; i < mEdgeVector.size(); i++)
    {
        mCongestionVector.push_back(0);
        mCountVector.push_back(-1);
    }
}

vertexGrid ConnectionGraph::getGrid(void)
{
    return mGrid;
}

std::vector<int> ConnectionGraph::getVertexVector(void)
{
    return mVertexVector;
}

std::vector<int> ConnectionGraph::getEdgeVector(void)
{
    return mEdgeVector;
}

int * ConnectionGraph::getVertexArrayPointer(void)
{
    return mVertexVector.data();
}

int * ConnectionGraph::getEdgeArrayPointer(void)
{
    return mEdgeVector.data();
}

int ConnectionGraph::getVertexArraySize(void)
{
    return static_cast<int>(mVertexVector.size());
}

int ConnectionGraph::getEdgeArraySize(void)
{
    return static_cast<int>(mEdgeVector.size());
}

graphData_t ConnectionGraph::getGraphData(void)
{
    graphData_t data;
    
    data.vertexArrayPointer = getVertexArrayPointer();
    data.edgeArrayPointer = getEdgeArrayPointer();
    data.vertexArraySize = getVertexArraySize();
    data.edgeArraySize = getEdgeArraySize();
    
    return data;
}

int ConnectionGraph::getVertexIdFromPlacement(int col, int row)
{
    int gridCol, gridRow;
    
    // The placement gets expanded from the original placement coordinates, so we need to translate
    // The width that was used as maximization needs to be checked
    if(mSideLength == mPlacementWidth)
    {
        gridCol = mPadding + 1 + (2 * col);
        gridRow = mPadding + (mPlacementWidth - mPlacementHeight) + 1 + (2 * row);
    }
    else
    {
        gridCol = mPadding + (mPlacementHeight - mPlacementWidth) + 1 + (2 * col);
        gridRow = mPadding + 1 + (2 * row);
    }
    
    return mGrid[gridCol][gridRow].id;
}
