//
//  FileParser.hpp
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-24.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#ifndef FileParser_hpp
#define FileParser_hpp

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "Util.hpp"

// Net vector typdef
typedef std::vector<std::vector<unsigned int>> netVec;

// Col/row position struct
typedef struct
{
    unsigned int                col;                    ///< Column
    unsigned int                row;                    ///< Row
} posStruct_t;

// Position vector typedef
typedef std::vector<posStruct_t> posVec;

// Parsed input struct
typedef struct
{
    unsigned int                numRows;                ///< Number of parsed rows
    unsigned int                numCols;                ///< Number of parsed columns
    
    unsigned int                numNodes;               ///< Number of nodes to place
    unsigned int                numConnections;         ///< The number of connections
    
    netVec                      nets;                   ///< Parsed nets
    posVec                      placement;              ///< Parsed placement information for the nodes
    
} parsedInputStruct_t;

class FileParser
{
public:
    FileParser(std::string netFilename, std::string placementFilename);
    ~FileParser(void);
    
    void setFilenames(std::string netFilename, std::string placementFilename);
    bool parseInput(void);
    parsedInputStruct_t getParsedInput(void);

private:
    bool parseNetFile(void);
    bool parsePlacementFile(void);
    
    parsedInputStruct_t mParsedInput;
    std::string mNetFilename;
    std::string mPlacementFilename;
};

#endif /* FileParser_hpp */
