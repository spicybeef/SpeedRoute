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
#include <chrono>

int Util_GetRandomInt(int i);
std::vector<std::string> Util_SplitString(std::string inString, char delimiter);

void Util_RecordStartTime(void);
double Util_GetElapsedTime(void);
void Util_RecordFinishTime(void);
double Util_GetFinishTime(void);

#endif /* Util_hpp */
