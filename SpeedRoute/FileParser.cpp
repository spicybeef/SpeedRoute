//
//  FileParser.cpp
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-24.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#include "FileParser.hpp"
#include "Types.h"

int g_enlargementFactor = SPACE_ENLARGEMENT_FACTOR;

FileParser::FileParser(std::string netFilename, std::string placementFilename)
{
    mNetFilename = netFilename;
    mPlacementFilename = placementFilename;
    if(g_enlargementFactor > 1)
    {
        std::cout << "Warning, space enlargement factor is greater than 1! (" << g_enlargementFactor << ")" << std::endl << std::endl;
    }
}

FileParser::~FileParser(void)
{
    
}

void FileParser::setFilenames(std::string netFilename, std::string placementFilename)
{
    mNetFilename = netFilename;
    mPlacementFilename = placementFilename;
}

bool FileParser::parseInput(void)
{
    bool rv = true;
    
    // Try parsing the net file
    if(!parseNetFile())
    {
        rv = false;
    }
    // Now try parsing the placement file
    if(!parsePlacementFile())
    {
        rv = false;
    }
    
    return rv;
}

parsedInputStruct_t FileParser::getParsedInput(void)
{
    return mParsedInput;
}

bool FileParser::parseNetFile(void)
{
    unsigned int i, j, numNodes;
    std::string line;
    std::vector<std::string> stringVec;
    
    std::ifstream inputFile(mNetFilename, std::ios::in);
    
    // Check if file was opened properly
    std::cout << "Parsing net file..." << std::endl;
    if (inputFile.is_open())
    {
        std::cout << "File " << mNetFilename << " opened! Here's what's in it:" << std::endl;
    }
    else
    {
        std::cout << "FATAL ERROR! File " << mNetFilename << " could not be opened!" << std::endl;
        return false;
    }
    
    // Get number of cells, number of nets, and grid size
    std::getline(inputFile, line);
    stringVec = Util_SplitString(line, ' ');
    mParsedInput.numNodes = stoi(stringVec[0]);
    mParsedInput.numConnections = stoi(stringVec[1]);
    mParsedInput.numRows = stoi(stringVec[2]) * g_enlargementFactor;
    mParsedInput.numCols = stoi(stringVec[3]) * g_enlargementFactor;
    
    std::cout << "Grid size is: " << mParsedInput.numRows << " rows x " << mParsedInput.numCols << " cols" << std::endl;
    std::cout << "Number of nodes is: " << mParsedInput.numNodes << std::endl;
    std::cout << "Number of connections is: " << mParsedInput.numConnections << std::endl << std::endl;
    
    // Get all connections
    for (i = 0; i < mParsedInput.numConnections; i++)
    {
        std::getline(inputFile, line);
        stringVec = Util_SplitString(line, ' ');
        // Get number of nodes for this net
        numNodes = stoi(stringVec[0]);
        // Now get all nodes for this net
        // Push back a new vector for this
        mParsedInput.nets.push_back(std::vector<int>());
        for (j = 0; j < numNodes; j++)
        {
            mParsedInput.nets[i].push_back(stoi(stringVec[j + 1]));
        }
    }
    
    inputFile.close();
    
    return true;
}

bool FileParser::parsePlacementFile(void)
{
    unsigned int i;
    std::string line;
    std::vector<std::string> stringVec;
    posStruct_t tempPos;
    
    std::ifstream inputFile(mPlacementFilename, std::ios::in);
    
    // Check if file was opened properly
    std::cout << "Parsing placement file..." << std::endl;
    if (inputFile.is_open())
    {
        std::cout << "File " << mPlacementFilename << " opened!" << std::endl;
    }
    else
    {
        std::cout << "FATAL ERROR! File " << mPlacementFilename << " could not be opened!" << std::endl;
        return false;
    }
    
    // There will be exactly enough placements, so iterate through the expected amount
    for(i = 0; i < mParsedInput.numNodes; i++)
    {
        std::getline(inputFile, line);
        stringVec = Util_SplitString(line, ' ');
        
        // Row comes first followed by column
        tempPos.row = stoi(stringVec[1]) * g_enlargementFactor;
        tempPos.col = stoi(stringVec[2]) * g_enlargementFactor;
        
        // Push back a placement
        mParsedInput.placement.push_back(tempPos);
    }
    std::cout << std::endl;
    inputFile.close();
    
    return true;
}
