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
#include "OpenClApp.h"
}

#include "Types.h"

static int g_sideLength;

void outputGrid(int * weightArray)
{
    std::ofstream myOutputFile("grid.csv", std::ios::out);
    
    for(int row = 0; row < g_sideLength; row++)
    {
        for(int col = 0; col < g_sideLength; col++)
        {
            if(weightArray[col + row * g_sideLength] == 0)
            {
                myOutputFile << ",";
            }
            else
            {
                myOutputFile << std::to_string(weightArray[col + row * g_sideLength]) << ",";
            }
        }
        myOutputFile << std::endl;
    }
    
    myOutputFile.close();
}

int main(int argc, char *argv[])
{
    // Validate and grab program options
    ProgramOptions programOptions(argc, argv);
    programOptions.validate();
    programOptions_t options = programOptions.getOptions();
    
    // Set debug level
    g_debugLevel = options.debugLevel;
    // Set enlargement factor
    g_enlargementFactor = options.enlargementFactor;
    
    // Parse the net and placement files
    FileParser fileParse(options.netFilenameIn, options.placementFilenameIn);
    if(!fileParse.parseInput())
    {
        return EXIT_FAILURE;
    }
    parsedInputStruct_t input = fileParse.getParsedInput();
    
    // Instantiate the window and graphics object
    sf::RenderWindow window(sf::VideoMode(
                                          static_cast<unsigned int>(WIN_VIEWPORT_WIDTH),
                                          static_cast<unsigned int>(WIN_VIEWPORT_HEIGHT)),
                            "SpeedRoute", sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);
    
    // Instantiate a new connection graph using the placement's col, row, and some padding
    ConnectionGraph connectionGraph(input.placement, input.numCols, input.numRows, ARCH_PADDING);
    // Grab the C-friendly graph data
    graphData_t graphData = connectionGraph.getGraphData();
    netData_t netData = connectionGraph.getNetVectors(input.nets, input.placement);

    // If OpenCL is being used, get a queue pointer and setup the memory structures
    if(options.openClEnableFlag)
    {
        OpenCl_Init(options.openClDeviceId);
        OpenCl_GraphWalk_InitGraphArrays(
                                         graphData.vertexArrayPointer,
                                         graphData.edgeArrayPointer,
                                         graphData.vertexArraySize,
                                         graphData.edgeArraySize
                                         );
    }
    
    // Update arch side length
    g_sideLength = graphData.sideLength;
    
    std::cout << "Routing starting..." << std::endl << std::endl;
    // Record start time
    auto start = std::chrono::high_resolution_clock::now();
    
    // Initialize the graph walker arrays
    GraphWalk_InitWalkData(graphData, netData, ARCH_CHANNEL_WIDTH);
    // Do routing in a separate thread
    sf::Thread routingThread(&GraphWalk_Main, options.openClEnableFlag);
    routingThread.launch();
    
    // Start a renderer in another thread if we're not in CLI mode
    if(options.programVisualMode != MODE_CLI)
    {
        Graphics graphics(&window, connectionGraph.getGrid(), graphData.sideLength);
        graphics.setRenderMode(options.programVisualMode);
        // Deactive the window's OpenGL context
        window.setActive(false);
        sf::Thread renderThread(&Graphics::render, &graphics);
        renderThread.launch();
        
        bool checkForRunning = true;
        while(window.isOpen())
        {
            graphics.processEvents();
            
            if(checkForRunning)
            {
                if(!GraphWalk_IsRoutingRunning())
                {
                    // No longer check
                    checkForRunning = false;
                    // Record end time
                    auto finish = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> elapsed = finish - start;
                    std::cout << "Routing finished in: " << elapsed.count() << " s" << std::endl;
                    // Output grid
                    GraphWalk_DebugPrintGrid(PRIO_LOW, const_cast<char *>("Final weights"), GraphWalk_GetWeightArray());
                    outputGrid(GraphWalk_GetWeightArray());
                    // Output routes
                    GraphWalk_DebugPrintRoutes(PRIO_LOW);
                }
            }
        }
        // If we're here we've terminated
        renderThread.terminate();
        routingThread.terminate();
    }
    else
    {

        while(GraphWalk_IsRoutingRunning())
        {
            // Wait until routing has finished...
        }
        
        // Record end time
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "Routing finished in: " << elapsed.count() << " s" << std::endl;
        // Output grid
        GraphWalk_DebugPrintGrid(PRIO_LOW, const_cast<char *>("Final weights"), GraphWalk_GetWeightArray());
        outputGrid(GraphWalk_GetWeightArray());
        // Output routes
        GraphWalk_DebugPrintRoutes(PRIO_LOW);
    }
    
    GraphWalk_FreeWalkData();
    
    return EXIT_SUCCESS;
}
