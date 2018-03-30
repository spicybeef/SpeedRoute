//
//  ProgramOptions.cpp
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-25.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#include "ProgramOptions.hpp"
#include "OpenClApp.h"

#define DEBUG_TO_BENCHMARKS_FOLDER_PATH "../../../../../benchmarks/"
#define DEFAULT_NET_FILE "apex4.txt"
#define DEFAULT_PLACEMENT_FILE "apex4_placement.txt"

ProgramOptions::ProgramOptions(int argc, char *argv[]):
    mDesc("Usage")
{
    std::string defaultPath = DEBUG_TO_BENCHMARKS_FOLDER_PATH;
    std::string defaultNetFile = DEFAULT_NET_FILE;
    std::string defaultPlacementFile = DEFAULT_PLACEMENT_FILE;
    
    try
    {
        mDesc.add_options()
        ("help", "produce help message")
        ("netfile", po::value<std::string>(&(mProgramOptions.netFilenameIn))->default_value(defaultPath + defaultNetFile), "the input net file")
        ("placementfile", po::value<std::string>(&(mProgramOptions.placementFilenameIn))->default_value(defaultPath + defaultPlacementFile), "the input placement file")
        ("openclinfo", po::bool_switch(&(mProgramOptions.openClInfoFlag)), "displays the OpenCL device support summary");
        
        po::store(po::command_line_parser(argc, argv).
                  options(mDesc).positional(mPosOptDesc).run(), mVarMap);
        po::notify(mVarMap);
    }
    catch (const std::exception &exc)
    {
        // catch anything thrown within try block that derives from std::exception
        std::cerr << exc.what() << std::endl;
        return;
    }
}

void ProgramOptions::validate(void)
{
    if (mVarMap.count("help"))
    {
        std::cout << mDesc << std::endl;
        return;
    }
    if (mProgramOptions.openClInfoFlag)
    {
        OpenCl_DeviceWalk();
        return;
    }
    if (mVarMap.count("netfile"))
    {
        std::cout << "Net file: "
        << mVarMap["netfile"].as<std::string>() << std::endl;
    }
    if (mVarMap.count("placementfile"))
    {
        std::cout << "Placement file: "
        << mVarMap["placementfile"].as<std::string>() << std::endl;
    }
}

programOptions_t ProgramOptions::getOptions(void)
{
    return mProgramOptions;
}
