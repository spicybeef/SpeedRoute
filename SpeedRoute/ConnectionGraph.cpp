//
//  ConnectionGraph.cpp
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-25.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#include "ConnectionGraph.hpp"

ConnectionGraph::ConnectionGraph(posVec placement, int numCol, int numRows, int padding)
{
    mPlacement = placement;
    mPlacementWidth = numCol;
    mPlacementHeight = numRows;
    mPadding = padding;
    
    // Generate the graph based on the passed in placement length
    if(mPlacementHeight > mPlacementWidth)
    {
        mSideLength = (2 * mPlacementHeight) + 1 + (mPadding * 2);
    }
    else
    {
        mSideLength = (2 * mPlacementWidth) + 1 + (mPadding * 2);
    }
    
    // Generate the graph and the blockage
    generateGraph();
    generateBlockage();
}

ConnectionGraph::~ConnectionGraph(void)
{
    
}

void ConnectionGraph::generateGraph(void)
{
    std::vector<vertex_t> tempCol;
    vertex_t tempVertex;
    vertex_t *tempVertexPointer;
    
    // Populate the grid
    for(int col = 0; col < mSideLength; col++)
    {
        // Clear out the temporary column
        tempCol.clear();
        // Fill up a row
        for(int row = 0; row < mSideLength; row++)
        {
            tempVertex.id = col + (row * mSideLength);
            
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
    for(int col = 0; col < mSideLength; col++)
    {
        for(int row = 0; row < mSideLength; row++)
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
    for(int col = 0; col < mSideLength; col++)
    {
        for(int row = 0; row < mSideLength; row++)
        {
            // Push back the current size of the edge vector, this determines the index to use in it
            mVertexVector.push_back(static_cast<int>(mEdgeVector.size()));
            for(int dir = DIR_NORTH; dir < DIR_NUM; dir++)
            {
                // For every neighbour, push back a corresponding edge
                if(mGrid[col][row].neighbour[dir] != -1)
                {
                    mEdgeVector.push_back(mGrid[col][row].neighbour[dir]);
                }
            }
        }
    }
}

void ConnectionGraph::generateBlockage()
{
    // Create a blockage vector the same size as our vertex vector and initialize to 0
    std::vector<int> blockage(mVertexVector.size(), 0);

    // Blockage corresponds to all possible vertex placements
    for(int row = 0; row < mSideLength; row++)
    {
        for(int col = 0; col < mSideLength; col++)
        {
            // Logic exists at odd intersections of row and col
            if((row % 2 == 1) && (col % 2 == 1))
            {
                blockage[mGrid[col][row].id] = 1;
            }
        }
    }
    
    mBlockageVector = blockage;
}

vertexGrid ConnectionGraph::getGrid(void)
{
    return mGrid;
}

graphData_t ConnectionGraph::getGraphData(void)
{
    graphData_t data;
    
    data.vertexArrayPointer = mVertexVector.data();
    data.edgeArrayPointer = mEdgeVector.data();
    data.blockageArrayPointer = mBlockageVector.data();
    data.vertexArraySize = static_cast<int>(mVertexVector.size());
    data.edgeArraySize = static_cast<int>(mEdgeVector.size());
    data.blockageArraySize = static_cast<int>(mBlockageVector.size());
    data.sideLength = mSideLength;
    
    std::cout << "Graph data:" << std::endl;
    std::cout << "Vertex size:" << data.vertexArraySize << std::endl;
    std::cout << "Edge size:" << data.edgeArraySize << std::endl;
    std::cout << "Blockage size:" << data.blockageArraySize << std::endl;
    
    return data;
}

int ConnectionGraph::getVertexIdFromPlacement(int col, int row)
{
    int gridCol, gridRow;
    
    // The placement gets expanded from the original placement coordinates, so we need to translate
    // The width that was used as maximization needs to be checked
    if(mPlacementWidth > mPlacementHeight)
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

netData_t ConnectionGraph::getNetVectors(netVec rawNets, posVec rawPos)
{
    mNetIdVector.clear();
    mNetVertexVector.clear();
    
    // Convert the raw nets and raw positions into a new netVec using vertex IDs instead
    int currentVertexIndex = 0;
    for(int i = 0; i < rawNets.size(); i++)
    {
        mNetIdVector.push_back(currentVertexIndex);
        for(int j = 0; j < rawNets[i].size(); j++)
        {
            // Get the original placement col and row positions
            posStruct_t pos = rawPos[rawNets[i][j]];
            // Convert the col and row into a vertex id and push it back to the current net's vector
            mNetVertexVector.push_back(getVertexIdFromPlacement(pos.col, pos.row));
            // Increment the current vertex index
            currentVertexIndex++;
        }
    }
    
    netData_t nets;
    
    nets.netIdArrayPointer = mNetIdVector.data();
    nets.netVertexArrayPointer = mNetVertexVector.data();
    nets.netIdArraySize = static_cast<int>(mNetIdVector.size());
    nets.netVertexArraySize = static_cast<int>(mNetVertexVector.size());
    
    return nets;
}
