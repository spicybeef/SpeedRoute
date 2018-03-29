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

#ifdef __APPLE__
#include "ResourcePath.hpp"
#else /* __APPLE__ */
string ResourcePath(void)
{
    return "";
}
#endif /*__APPLE__ */


class Graphics
{
// Public members
public:
    Graphics(void);
    void run(void);
    
// Private members
private:
    void processEvents(void);
    void update(void);
    void render(void);
    
    // Windows object
    sf::RenderWindow mWindow;
    // Icon
    sf::Image mIcon;
    // Texture
    sf::Texture mTexture;
    sf::Sprite mSprite;
    
    // Graphics constants
    static const unsigned int WINDOW_WIDTH = 1920;
    static const unsigned int WINDOW_HEIGHT = 1080;
    static const unsigned int FRAMERATE_LIM = 60;
};

#endif /* Graphics_hpp */
