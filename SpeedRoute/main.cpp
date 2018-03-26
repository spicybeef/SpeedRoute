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

static const int ARCH_PADDING = 4;

int main(int argc, char *argv[])
{
    std::string netFilenameIn;
    std::string placementFilenameIn;
    bool openClInfoFlag = false;
    programOptions_t options;
    parsedInputStruct_t input;
    int architectureSideLength;
    
    ProgramOptions programOptions(argc, argv);
    FileParser fileParse;
    ConnectionGraph connectionGraph;
    Graphics graphics;

    // Validate and grab program options
    programOptions.validate();
    options = programOptions.getOptions();
    
    // Parse the net and placement files
    fileParse.setFilenames(options.netFilenameIn, options.placementFilenameIn);
    fileParse.parseInput();
    input = fileParse.getParsedInput();
    
    // Generate the graph based on the longest edge
    if(input.numCols > input.numRows)
    {
        architectureSideLength = (2 * input.numCols) + 1 + (ARCH_PADDING * 2);
    }
    else
    {
        architectureSideLength = (2 * input.numRows) + 1 + (ARCH_PADDING * 2);
    }
    connectionGraph.generateGraph(architectureSideLength);
    
    // Run the graphics for the router
    graphics.run();
    
    return EXIT_SUCCESS;
}
