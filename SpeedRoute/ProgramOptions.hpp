//
//  ProgramOptions.hpp
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-25.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#ifndef ProgramOptions_hpp
#define ProgramOptions_hpp

#include <string>
#include <iostream>
#include <boost/program_options.hpp>

extern "C"
{
#include "GraphWalk.h"
#include "OpenClApp.h"
}

#include "Types.h"

namespace po = boost::program_options;

typedef struct
{
    std::string netFilenameIn;
    std::string placementFilenameIn;
    
    bool openClInfoFlag;
    bool openClEnableFlag;
    unsigned int programVisualMode;
    unsigned int openClDeviceId;
    unsigned int debugLevel;
    unsigned int enlargementFactor;
    
} programOptions_t;

class ProgramOptions
{
    
public:
    ProgramOptions(int argc, char *argv[]);
    
    void validate(void);
    programOptions_t getOptions(void);
    
private:
    po::options_description mDesc;
    po::variables_map mVarMap;
    po::positional_options_description mPosOptDesc;
    
    programOptions_t mProgramOptions;
};

#endif /* ProgramOptions_hpp */
