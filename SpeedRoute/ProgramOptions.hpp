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

namespace po = boost::program_options;

typedef struct
{
    std::string netFilenameIn;
    std::string placementFilenameIn;
    
    bool openClInfoFlag;
    
} programOptions_t;

class ProgramOptions
{
    
public:
    ProgramOptions(int argc, char *argv[]);
    
    void validate(void);
    
private:
    po::options_description mDesc;
    po::variables_map mVarMap;
    po::positional_options_description mPosOptDesc;
    
    programOptions_t programOptions;
};

#endif /* ProgramOptions_hpp */
