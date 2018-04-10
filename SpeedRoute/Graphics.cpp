//
//  Graphics.cpp
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-25.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#include "Graphics.hpp"
#include "OpenClApp.h"

Graphics::Graphics(sf::RenderWindow * window, vertexGrid grid, int sideLength, programOptions_t options) :
    mWindow(window),
    mViewportSize(
                  static_cast<unsigned int>(WIN_VIEWPORT_WIDTH), static_cast<unsigned int>(WIN_VIEWPORT_HEIGHT))
{
    // Set a framerate limit
    mWindow->setFramerateLimit(FRAMERATE_LIM);
    // Get grid structure
    mGrid = grid;
    // Get side length
    mSideLength = sideLength;
    // Initialize cell properties
    initializeCellProperties();
    // Load general font
    mFont.loadFromFile("consola.ttf");
    // Load console font
    mLogFont.loadFromFile("consola.ttf");
    // Set log text
    mLogText.setFont(mLogFont);
    mLogText.setCharacterSize(17);
    mLogText.setFillColor(sf::Color::Green);
    mLogText.setStyle(sf::Text::Regular);
    mLogText.setPosition(sf::Vector2f(0.f + WIN_INFOPORT_PADDING, WIN_VIEWPORT_HEIGHT - WIN_INFOPORT_HEIGHT + WIN_INFOPORT_PADDING));
    // Set program options
    mOptions = options;
}

void Graphics::processEvents(void)
{
    sf::Event event;
    while (mWindow->pollEvent(event))
    {
        switch (event.type)
        {
            case sf::Event::Closed:
                mWindow->close();
                break;
            case sf::Event::Resized:
                mWindow->setView(calcView(sf::Vector2u(event.size.width, event.size.height), mViewportSize));
                break;
            case sf::Event::LostFocus:
                break;
            case sf::Event::GainedFocus:
                break;
            case sf::Event::TextEntered:
                break;
            case sf::Event::KeyPressed:
                break;
            case sf::Event::KeyReleased:
                break;
            case sf::Event::MouseWheelMoved:
                break;
            case sf::Event::MouseWheelScrolled:
                break;
            case sf::Event::MouseButtonPressed:
                break;
            case sf::Event::MouseButtonReleased:
                break;
            case sf::Event::MouseMoved:
                break;
            case sf::Event::MouseEntered:
                break;
            case sf::Event::MouseLeft:
                break;
            case sf::Event::JoystickButtonPressed:
                break;
            case sf::Event::JoystickButtonReleased:
                break;
            case sf::Event::JoystickMoved:
                break;
            case sf::Event::JoystickConnected:
                break;
            case sf::Event::JoystickDisconnected:
                break;
            case sf::Event::TouchBegan:
                break;
            case sf::Event::TouchMoved:
                break;
            case sf::Event::TouchEnded:
                break;
            case sf::Event::SensorChanged:
                break;
            case sf::Event::Count:
                break;
            default:
                break;
        }
    }
}

void Graphics::update(void)
{
    
}

void Graphics::initializeCellProperties(void)
{
    float rowToColRatio, graphicportRatio, cellSize, cellOffset, cellOppositeOffset;
  
    // Use rows to fill vertically
    cellSize = WIN_GRAPHICPORT_HEIGHT / static_cast<float>(mSideLength);
    // Cell offset is always half of cell size
    cellOffset = cellSize / 2.f;
    cellOppositeOffset = cellOffset + (WIN_GRAPHICPORT_WIDTH - static_cast<float>(mSideLength) * cellSize) / 2.f;
    
    // Record the cell properties
    mCellProperties.cellSize = cellSize;
    mCellProperties.cellOffset = cellOffset;
    mCellProperties.cellOppositeOffset = cellOppositeOffset;
}


drawPosStruct_t Graphics::getGridCellCoordinate(int col, int row)
{
    drawPosStruct_t drawPos;
    
    drawPos.x = col * mCellProperties.cellSize + mCellProperties.cellOppositeOffset;
    drawPos.y = row * mCellProperties.cellSize + mCellProperties.cellOffset;
    
    return drawPos;
}

std::vector<sf::RectangleShape> Graphics::generateGridGeometries(void)
{
    std::vector<sf::RectangleShape> grid;
    unsigned int i, j;
    
    // Populate the grid vector with the data obtained above
    for (i = 0; i < mSideLength; i++)
    {
        for (j = 0; j < mSideLength; j++)
        {
            grid.push_back(sf::RectangleShape());

            grid.back().setPosition(
                                    static_cast<float>((i*mCellProperties.cellSize) + mCellProperties.cellOppositeOffset),
                                    static_cast<float>((j*mCellProperties.cellSize) + mCellProperties.cellOffset)
                                    );
            grid.back().setSize(sf::Vector2f(mCellProperties.cellSize * GRID_SHRINK_FACTOR, mCellProperties.cellSize * GRID_SHRINK_FACTOR));
            grid.back().setOrigin(sf::Vector2f(mCellProperties.cellSize * GRID_SHRINK_FACTOR * 0.5f, mCellProperties.cellSize * GRID_SHRINK_FACTOR * 0.5f));
            grid.back().setFillColor(sf::Color::White);
        }
    }
    
    return grid;
}

std::vector<std::vector<std::vector<sf::Vertex>>> Graphics::generateNetGeometries(void)
{
    std::vector<std::vector<std::vector<sf::Vertex>>> netGeometries;
    
    // Wait for lock
    GraphWalk_WaitLockNetsSegments();
    GraphWalk_LockNetSegments(true);
    netRoutes_t netRoutes = GraphWalk_GetNetRoutes();

    // Net ID -> Segment ID -> Vertex ID
    for(int net = 0; net < netRoutes.netRouteArraySize; net++)
    {
        int segmentStart = netRoutes.netRouteArrayPointer[net];
        int segmentEnd = netRoutes.netRouteArraySize;
        
        std::vector<std::vector<sf::Vertex>> netSegments;
        for(int segment = segmentStart; segment < segmentEnd; segment++)
        {
            if(netRoutes.segmentIdArraySize == 0)
            {
                break;
            }
            int vertexStart = netRoutes.segmentIdArrayPointer[segment];
            int vertexEnd;
            if((segment + 1) < netRoutes.segmentIdArraySize)
            {
                vertexEnd = netRoutes.segmentIdArrayPointer[segment + 1];
            }
            else
            {
                vertexEnd = netRoutes.segmentIdArraySize;
            }
            
            std::vector<sf::Vertex> netSegment;
            for(int vertex = vertexStart; vertex < vertexEnd; vertex++)
            {
                if(netRoutes.segmentVertexArraySize == 0)
                {
                    break;
                }
                sf::Vector2f netVertex;
                int currentVertex = netRoutes.segmentVertexArrayPointer[vertex];
                // Column and row are function of vertex ID and architecture side length
                int col = currentVertex % mSideLength;
                int row = currentVertex / mSideLength;
                // Get vertex position
                drawPosStruct_t drawPos = getGridCellCoordinate(col, row);
                // Push back the vertex coordinate
                netVertex = sf::Vector2f(drawPos.x, drawPos.y);
                // Pick a color
                sf::Color color = sf::Color(0, 0, 255, 255);
                // Push back a vertex
                netSegment.push_back(sf::Vertex(netVertex, color));
            }
            // Push back the segment into the net's segments
            netSegments.push_back(netSegment);
        }
        netGeometries.push_back(netSegments);
    }
    // safe to change now
    GraphWalk_LockNetSegments(false);
    
    return netGeometries;
}

sf::Text Graphics::generateVertexText(int vertex)
{
    std::stringstream stringStream;
    sf::Text placedNodeText;
    
    stringStream.str(std::string());
    
    stringStream << vertex;
    placedNodeText.setFont(mFont);
    placedNodeText.setString(stringStream.str());
    int col = vertex % mSideLength;
    int row = vertex / mSideLength;
    placedNodeText.setPosition(
                                      static_cast<float>((col * mCellProperties.cellSize) + mCellProperties.cellOppositeOffset),
                                      static_cast<float>((row * mCellProperties.cellSize) + mCellProperties.cellOffset)
                                      );
    placedNodeText.setCharacterSize(9);
    placedNodeText.setFillColor(sf::Color::Black);
    placedNodeText.setStyle(sf::Text::Regular);
    placedNodeText.setOrigin(sf::Vector2f(mCellProperties.cellSize * CELL_SHRINK_FACTOR * 0.5f, mCellProperties.cellSize * CELL_SHRINK_FACTOR * 0.5f));

    return placedNodeText;
}

void Graphics::setRenderMode(unsigned int mode)
{
    mRenderMode = mode;
}

void Graphics::setLogString(void)
{
    std::stringstream stringStream;
    
    stringStream << std::fixed << std::setprecision(3);
    stringStream << "Routing Net:   " << std::setw(9) << GraphWalk_GetCurrentNetNum() << "/" << std::setw(3) << GraphWalk_GetTotalNetNum() << "   ";
    stringStream << "Source Vertex: " << std::setw(9) << GraphWalk_GetCurrentSourceVertex() << "   ";
    stringStream << "Sink Vertex:   " << std::setw(9) << GraphWalk_GetCurrentSinkVertex() << "   ";
    stringStream << "Expansion:     " << std::setw(9) << GraphWalk_GetCurrentExpansion() << "   ";
    stringStream << std::endl;
    stringStream << "State:       ";
    switch (GraphWalk_GetCurrentState())
    {
        case STATE_IDLE:
            stringStream << "Idle.";
            break;
        case STATE_EXPANSION:
            stringStream << "Exansion           Elapsed time: " << std::setw(10) << Util_GetElapsedTime() << " s" << std::endl;
            break;
        case STATE_TRACEBACK:
            stringStream << "Traceback          Elapsed time: " << std::setw(10) << Util_GetElapsedTime() << " s" << std::endl;
            break;
        case STATE_ROUTE_SUCCESS:
            stringStream << "* ROUTE SUCCESS! * Elapsed time: " << std::setw(10) << Util_GetFinishTime() << " s" << std::endl;
            break;
        case STATE_ROUTE_FAILURE:
            stringStream << "* ROUTE FAILED! *  Elapsed time: " << std::setw(10) << Util_GetFinishTime() << " s" << std::endl;
            break;
        default:
            break;
    }
    stringStream << "Net file:       " << mOptions.netFilenameIn << std::endl;
    stringStream << "Placement file: " << mOptions.placementFilenameIn << std::endl;
    
    mLogContents = stringStream.str();
}

void Graphics::render(void)
{
    // Background
    sf::RectangleShape background(sf::Vector2f(WIN_GRAPHICPORT_WIDTH, WIN_GRAPHICPORT_HEIGHT));
    background.setPosition(sf::Vector2f(0.f, 0.f));
    background.setFillColor(sf::Color(200, 200, 200, 255));
    mWindow->setView(calcView(mWindow->getSize(), mViewportSize));
    auto grid = generateGridGeometries();
    // Render at specified rate
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<std::vector<std::vector<sf::Vertex>>> netGeometries;
    while(mWindow->isOpen())
    {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = now - start;
        
        // Always draw background + grid
        mWindow->clear();
        // Draw background
        mWindow->draw(background);
        // Draw grid
        for(int i = 0; i < grid.size(); i++)
        {
            mWindow->draw(grid[i]);
            // mWindow->draw(generateVertexText(i));
        }
        // Draw log
        setLogString();
        mLogText.setString(mLogContents);
        mWindow->draw(mLogText);
        for(int net = 0; net < netGeometries.size(); net++)
        {
            for(int segment = 0; segment < netGeometries[net].size(); segment++)
            {
                mWindow->draw((sf::Vertex*)(&netGeometries[net][segment].front()), netGeometries[net][segment].size(), sf::LinesStrip);
            }
        }
        // Check if we're only showing the end result
        if(mRenderMode == MODE_VISUAL_END_RESULT && GraphWalk_IsRoutingRunning())
        {
            mWindow->display();
            continue;
        }
        else if(elapsed.count() > RENDER_PERIOD_S || mRenderMode == MODE_VISUAL_END_RESULT)
        {
            // Draw nets
            netGeometries = generateNetGeometries();
            start = std::chrono::high_resolution_clock::now();
        }
        
        mWindow->display();
    }
}

sf::View Graphics::calcView(const sf::Vector2u &windowSize, const sf::Vector2u &mViewportSize)
{
    sf::FloatRect viewport(0.f, 0.f, 1.f, 1.f);
    float viewportWidth = static_cast<float>(mViewportSize.x);
    float viewportHeight = static_cast<float>(mViewportSize.y);
    float screenwidth = windowSize.x / static_cast<float>(mViewportSize.x);
    float screenheight = windowSize.y / static_cast<float>(mViewportSize.y);
    
    if (screenwidth > screenheight)
    {
        viewport.width = screenheight / screenwidth;
        viewport.left = (1.f - viewport.width) / 2.f;
    }
    else if (screenwidth < screenheight)
    {
        viewport.height = screenwidth / screenheight;
        viewport.top = (1.f - viewport.height) / 2.f;
    }
    
    sf::View view(sf::FloatRect(0.f, 0.f, viewportWidth, viewportHeight));
    view.setViewport(viewport);
    
    return view;
}
