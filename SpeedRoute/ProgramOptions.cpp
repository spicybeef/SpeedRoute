//
//  ProgramOptions.cpp
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-25.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#include "ProgramOptions.hpp"
#include "OpenClApp.h"
#include "Types.h"

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
        ("clenable", po::bool_switch(&(mProgramOptions.openClEnableFlag)), "enabled the program to be accelerated by OpenCL")
        ("cldevice", po::value<unsigned int>(&(mProgramOptions.openClDeviceId))->default_value(0), "the OpenCL device ID to use")
        ("clinfo", po::bool_switch(&(mProgramOptions.openClInfoFlag)), "displays the OpenCL device support summary")
        ("debug", po::value<unsigned int>(&(mProgramOptions.debugLevel))->default_value(PRIO_HIGH), "the debug level to use (0: PRIO_LOW, 1: PRIO_NORM, 2: PRIO_HIGH)")
        ("enlargement", po::value<unsigned int>(&(mProgramOptions.enlargementFactor))->default_value(SPACE_ENLARGEMENT_FACTOR), "the problem space enlargement factor");
        
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
    if (mProgramOptions.openClEnableFlag)
    {
        std::cout << "OpenCL enabled!" << std::endl;
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
    std::cout << "Current debug priority level: " << mProgramOptions.debugLevel << std::endl;
    std::cout << std::endl;
}

programOptions_t ProgramOptions::getOptions(void)
{
    return mProgramOptions;
}
