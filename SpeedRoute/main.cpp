// C++ Includes
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>

// Boost includes
#include <boost/program_options.hpp>
namespace po = boost::program_options;

// SFML Includes
#include <SFML/Graphics.hpp>

#include "OpenCLApp.h"

int main(int argc, char *argv[])
{
    std::string netFilenameIn;
    std::string placementFilenameIn;
    bool openClInfoFlag = false;
    
    // Declare the supported options
    po::options_description desc("Usage");
    po::variables_map vm;
    po::positional_options_description p;
    
    try
    {
        desc.add_options()
        ("help", "produce help message")
        ("netfile", po::value<std::string>(&netFilenameIn)->default_value("..\\benchmarks\\C880.txt"), "the input net file")
        ("placementfile", po::value<std::string>(&placementFilenameIn)->default_value("..\\benchmarks\\C880_placement.txt"), "the input placement file")
        ("openclinfo", po::bool_switch(&openClInfoFlag), "displays the OpenCL device support summart");
        
        po::store(po::command_line_parser(argc, argv).
                  options(desc).positional(p).run(), vm);
        po::notify(vm);
    }
    catch (const std::exception &exc)
    {
        // catch anything thrown within try block that derives from std::exception
        std::cerr << exc.what() << std::endl;
        return -1;
    }
    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        return 1;
    }
    if (openClInfoFlag)
    {
        OpenCL_DeviceWalk();
        return 1;
    }
    if (vm.count("netfile"))
    {
        std::cout << "Net file: "
        << vm["netfile"].as<std::string>() << std::endl;
    }
    if (vm.count("placementfile"))
    {
        std::cout << "Placement file: "
        << vm["placementfile"].as<std::string>() << std::endl;
    }

    return EXIT_SUCCESS;
}
