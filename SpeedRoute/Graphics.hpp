//
//  Graphics.hpp
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-25.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#ifndef Graphics_hpp
#define Graphics_hpp

#include "SFML/Graphics.hpp"

#define WIDTH 1000
#define HEIGHT 500

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
    sf::RenderWindow window;
    
    const unsigned int WINDOW_WIDTH = 1920;
    const unsigned int WINDOW_HEIGHT = 1080;
    const unsigned int FRAMERATE_LIM = 60;
};

#endif /* Graphics_hpp */