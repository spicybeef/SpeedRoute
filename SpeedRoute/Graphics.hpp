//
//  Graphics.hpp
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-25.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#ifndef Graphics_hpp
#define Graphics_hpp

#include <iostream>

#include "SFML/Graphics.hpp"
#include "ConnectionGraph.hpp"
#include "FileParser.hpp"
#include "Types.h"

extern "C"
{
#include "GraphWalk.h"
#include "OpenClApp.h"
}

// Drawing x/y position struct
typedef struct
{
    float                                       x;                      ///< X coordinate
    float                                       y;                      ///< Y coordinate
} drawPosStruct_t;

// Net segment struct
typedef struct
{
    unsigned int                                net;                    ///< Net segment's net id
    std::vector<unsigned int>                   nodes;                  ///< Nodes on either side of the segment
} netSegmentStruct_t;

// Net struct
typedef struct Net
{
    unsigned int                                id;                     ///< Net's id
    std::vector<unsigned int>                   nodes;                  ///< Pointers to the net's nodes
    std::vector<unsigned int>                   segments;               ///< Individual segments of the net
    sf::Color                                   color;                  ///< Net color
} netStruct_t;

class Graphics
{
// Public members
public:
    Graphics(sf::RenderWindow * window, vertexGrid grid, int sideLength);
    void render(void);
    void processEvents(void);
    std::vector<sf::RectangleShape> generateGridGeometries(void);
    std::vector<std::vector<std::vector<sf::Vertex>>> generateNetGeometries();
    sf::Text generateVertexText(int vertex);
    bool terminated(void);
    
// Private members
private:
    void update(void);
    sf::View calcView(const sf::Vector2u &windowSize, const sf::Vector2u &viewportSize);
    void initializeCellProperties(void);
    drawPosStruct_t getGridCellCoordinate(int col, int row);
    
    // Window
    sf::RenderWindow * mWindow;
    // Viewport size
    const sf::Vector2u mViewportSize;
    // Architecture variables
    cellPropertiesStruct_t mCellProperties;
    vertexGrid mGrid;
    int mSideLength;
    // Geometry
    std::vector<sf::RectangleShape> mBackgroundGrid;
    sf::Font mFont;
    
    bool mTerminated;
    
    // Graphics constants
    static const unsigned int WINDOW_WIDTH = 1920;
    static const unsigned int WINDOW_HEIGHT = 1080;
    static const unsigned int FRAMERATE_LIM = 60;
};

#endif /* Graphics_hpp */
