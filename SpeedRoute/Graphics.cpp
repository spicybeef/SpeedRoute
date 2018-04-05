//
//  Graphics.cpp
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-25.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#include "Graphics.hpp"
#include "OpenClApp.h"
#include "GraphWalk.h"

Graphics::Graphics(sf::RenderWindow * window, vertexGrid grid, int sideLength) :
    mWindow(window),
    mViewportSize(
                  static_cast<unsigned int>(WIN_VIEWPORT_WIDTH), static_cast<unsigned int>(WIN_VIEWPORT_HEIGHT)),
    mTerminated(false)
{
    // Set a framerate limit
    mWindow->setFramerateLimit(FRAMERATE_LIM);
    // Get grid structure
    mGrid = grid;
    // Get side length
    mSideLength = sideLength;
    // Initialize cell properties
    initializeCellProperties();
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
                mTerminated = true;
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

std::vector<std::vector<std::vector<sf::Vertex>>> Graphics::generateNetGeometries()
{
    // Get the latest route data, safely
    sf::Mutex mutex;
    while(GraphWalk_IsTracingBack());
    mutex.lock();
    netRoutes_t netRoutes = GraphWalk_GetNetRoutes();
    mutex.unlock();

    std::vector<std::vector<std::vector<sf::Vertex>>> netGeometries;
    
    // Net ID -> Segment ID -> Vertex ID
    for(int net = 0; net < netRoutes.netRouteArraySize; net++)
    {
        std::vector<std::vector<sf::Vertex>> netSegments;
        for(int segment = 0; segment < netRoutes.segmentIdArraySize; segment++)
        {
            std::vector<sf::Vertex> netSegment;
            for(int vertex = 0; vertex < netRoutes.segmentVertexArraySize; vertex++)
            {
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
    
    return netGeometries;
}

bool Graphics::terminated(void)
{
    return mTerminated;
}

void Graphics::render(void)
{
    // Background
    sf::RectangleShape background(sf::Vector2f(WIN_GRAPHICPORT_WIDTH, WIN_GRAPHICPORT_HEIGHT));
    background.setPosition(sf::Vector2f(0.f, 0.f));
    background.setFillColor(sf::Color(200, 200, 200, 255));
    mWindow->setView(calcView(mWindow->getSize(), mViewportSize));
    auto grid = generateGridGeometries();
    while(mWindow->isOpen())
    {
        mWindow->clear();
        // Draw background
        mWindow->draw(background);
        // Draw grid
        for(int i = 0; i < grid.size(); i++)
        {
            mWindow->draw(grid[i]);
        }
        // Draw nets
        std::vector<std::vector<std::vector<sf::Vertex>>> netGeometries = generateNetGeometries();
        for(int net = 0; net < netGeometries.size(); net++)
        {
            for(int segment = 0; segment < netGeometries[net].size(); segment++)
            {
                mWindow->draw((sf::Vertex*)(&netGeometries[net][segment].front()), netGeometries[net][segment].size(), sf::Lines);
            }
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
