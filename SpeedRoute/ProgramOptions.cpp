//
//  ProgramOptions.cpp
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-25.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#include "ProgramOptions.hpp"
#include "OpenCLApp.h"

ProgramOptions::ProgramOptions(int argc, char *argv[]):
    mDesc("Usage")
{
    try
    {
        mDesc.add_options()
        ("help", "produce help message")
        ("netfile", po::value<std::string>(&(programOptions.netFilenameIn))->default_value("..\\benchmarks\\C880.txt"), "the input net file")
        ("placementfile", po::value<std::string>(&(programOptions.placementFilenameIn))->default_value("..\\benchmarks\\C880_placement.txt"), "the input placement file")
        ("openclinfo", po::bool_switch(&(programOptions.openClInfoFlag)), "displays the OpenCL device support summart");
        
        po::store(po::command_line_parser(argc, argv).
                  options(mDesc).positional(mPosOptDesc).run(), mVarMap);
        po::notify(mVarMap);
    }
    catch (const std::exception &exc)
    {
        // catch anything thrown within try block that derives from std::exception
        std::cerr << exc.what() << std::endl;
        return EXIT_FAILURE;
    }
}

void ProgramOptions::validate(void)
{
    if (mVarMap.count("help"))
    {
        std::cout << mDesc << std::endl;
        return EXIT_SUCCESS;
    }
    if (programOptions.openClInfoFlag)
    {
        OpenCl_DeviceWalk();
        return EXIT_SUCCESS;
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
