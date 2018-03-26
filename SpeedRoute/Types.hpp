//
//  Types.h
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-24.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#ifndef Types_h
#define Types_h

// typedef helpers to make things legible
typedef std::vector<std::vector<unsigned int>>  netVec;

// Col/row position struct
typedef struct
{
    unsigned int                col;                    ///< Column
    unsigned int                row;                    ///< Row
} posStruct_t;

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

#endif /* Types_h */
