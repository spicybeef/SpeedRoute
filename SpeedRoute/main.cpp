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

static const int ARCH_PADDING = 2;

int main(int argc, char *argv[])
{
    // Validate and grab program options
    ProgramOptions programOptions(argc, argv);
    programOptions.validate();
    programOptions_t options = programOptions.getOptions();
    
    // Parse the net and placement files
    FileParser fileParse(options.netFilenameIn, options.placementFilenameIn);
    if(!fileParse.parseInput())
    {
        return EXIT_FAILURE;
    }
    parsedInputStruct_t input = fileParse.getParsedInput();
    
    // Instantiate a new connection graph using the placement's col, row, and some padding
    ConnectionGraph connectionGraph(input.placement, input.numCols, input.numRows, ARCH_PADDING);
    // Grab the C-friendly graph data
    graphData_t graphData = connectionGraph.getGraphData();
    netData_t netData = connectionGraph.getNetVectors(input.nets, input.placement);


    // Initialize the graph walker arrays
    GraphWalk_InitWalkData(graphData, netData, 8);
    // Initialize the net status array
    GraphWalk_InitNetStatus();
    
    // Route nets
    // Initiailize the weight array
    GraphWalk_InitWeight();
    // Initialize the routing arrays
    GraphWalk_InitNetRoutes();
    for(int i = 0; i < input.nets.size(); i++)
    {
        // Init a new net route
        GraphWalk_NewNetRoute();
        // Route the nets
        if(!GraphWalk_RouteNet(i))
        {
            break;
        }
    }
    
    GraphWalk_DebugPrintGrid(PRIO_HIGH, const_cast<char *>("FInal weights"), GraphWalk_GetWeightArray());
    
    // Infinite loop for now
    for(;;);

    // Run the graphics for the router
    Graphics graphics;
    graphics.run();
    
    return EXIT_SUCCESS;
}
