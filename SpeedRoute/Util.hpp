//
//  Util.hpp
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-24.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#ifndef Util_hpp
#define Util_hpp

#include <vector>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <random>

#include <SFML/Graphics.hpp>

int getRandomInt(int i);
std::vector<std::string> splitString(std::string inString, char delimiter);
sf::View calcView(const sf::Vector2u &windowsize, const sf::Vector2u &designedsize);

#endif /* Util_hpp */
