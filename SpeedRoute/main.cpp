//
//  RouterGraphics.cpp
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-18.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

// Program Includes
#include "ResourcePath.hpp"
#include "ProgramOptions.hpp"
#include "FileParser.hpp"
#include "ConnectionGraph.hpp"
#include "Graphics.hpp"

extern "C"
{
#include "GraphWalk.h"
}

static const int ARCH_PADDING = 4;

int main(int argc, char *argv[])
{
    // Validate and grab program options
    ProgramOptions programOptions(argc, argv);
    programOptions.validate();
    programOptions_t options = programOptions.getOptions();
    
    // Parse the net and placement files
    FileParser fileParse(options.netFilenameIn, options.placementFilenameIn);
    fileParse.parseInput();
    parsedInputStruct_t input = fileParse.getParsedInput();
    
    ConnectionGraph connectionGraph(input.numCols, input.numRows, ARCH_PADDING);
    graphData_t graphData = connectionGraph.getGraphData();
    
    GraphWalk_Test(graphData);
    
    // Run the graphics for the router
    Graphics graphics;
    graphics.run();
    
    return EXIT_SUCCESS;
}
