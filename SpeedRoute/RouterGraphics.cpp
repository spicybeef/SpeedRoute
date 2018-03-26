//
//  RouterGraphics.cpp
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-25.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#include "RouterGraphics.hpp"

RouterGraphics::RouterGraphics(void) :
    window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Speed Route")
{
    window.setFramerateLimit(FRAMERATE_LIM);
}

void RouterGraphics::run(void)
{
    while (window.isOpen())
    {
        processEvents();
        update();
        render();
    }
}

void RouterGraphics::processEvents(void)
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        switch (event.type)
        {
            case sf::Event::Closed:
                window.close();
                break;
        }
    }
}

void RouterGraphics::update(void)
{
}

void RouterGraphics::render(void)
{
    window.clear();
    
    //draw()
    
    window.display();
}
