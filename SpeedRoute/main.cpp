//
//  RouterGraphics.cpp
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-18.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

// Program Includes
#include "ResourcePath.hpp"
#include "ProgramOptions.hpp"
#include "FileParser.hpp"
#include "Graphics.hpp"

int main(int argc, char *argv[])
{
    std::string netFilenameIn;
    std::string placementFilenameIn;
    bool openClInfoFlag = false;
    Graphics graphics;
    ProgramOptions programOptions(argc, argv);

    programOptions.validate();
    graphics.run();
    
    return EXIT_SUCCESS;
}
