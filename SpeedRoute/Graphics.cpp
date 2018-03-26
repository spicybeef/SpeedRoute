//
//  Graphics.cpp
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-25.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#include "Graphics.hpp"

Graphics::Graphics(void) :
    mWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Speed Route"),
    mIcon(),
    mTexture(),
    mSprite()
{
    // Set a framerate limit
    mWindow.setFramerateLimit(FRAMERATE_LIM);
    
    // Set the Icon
    if (!mIcon.loadFromFile(resourcePath() + "icon.png"))
    {
        return EXIT_FAILURE;
    }
    mWindow.setIcon(mIcon.getSize().x, mIcon.getSize().y, mIcon.getPixelsPtr());
    
    // Set the texture
    if (!mTexture.loadFromFile(resourcePath() + "cute_image.jpg"))
    {
        return EXIT_FAILURE;
    }
    mSprite.setTexture(mTexture);
}

void Graphics::run(void)
{
    while (mWindow.isOpen())
    {
        processEvents();
        update();
        render();
    }
}

void Graphics::processEvents(void)
{
    sf::Event event;
    while (mWindow.pollEvent(event))
    {
        switch (event.type)
        {
            case sf::Event::Closed:
                mWindow.close();
                break;
            case sf::Event::Resized:
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
    mWindow.clear();
    
    // Draw the sprite
    mWindow.draw(mSprite);
    
    mWindow.display();
}
