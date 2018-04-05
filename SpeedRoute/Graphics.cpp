//
//  Graphics.cpp
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-25.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#include "Graphics.hpp"
#include "Types.h"

Graphics::Graphics(sf::RenderWindow * window) :
    mWindow(window),
    mViewportSize(
                  static_cast<unsigned int>(WIN_VIEWPORT_WIDTH), static_cast<unsigned int>(WIN_VIEWPORT_HEIGHT))
{
    // Set a framerate limit
    mWindow->setFramerateLimit(FRAMERATE_LIM);
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

void Graphics::render(void)
{
    while(mWindow->isOpen())
    {
        mWindow->clear();
        update();
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
