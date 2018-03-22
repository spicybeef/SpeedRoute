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

// Program Includes
//#include "Types.h"
//#include "Partitioner.h"
//#include "NetList.h"
//#include "Util.h"
//#include "Types.h"
//#include "Constants.h"

int main(int argc, char *argv[])
{
    std::string filenameIn;
    
    // Declare the supported options
    po::options_description desc("Usage");
    po::variables_map vm;
    po::positional_options_description p;
    try
    {
        
        desc.add_options()
        ("help", "produce help message")
        ("input-file", po::value<std::string>(&filenameIn)->default_value("..\\benchmarks\\C880.txt"),
         "the input file");
//        ("program-mode,p", po::value<unsigned int>(&programModeIn)->default_value(static_cast<unsigned int>(PROGRAM_MODE_GUI)),
//         "set the program mode")
//        ("draw-mode,d", po::value<unsigned int>(&drawModeIn)->default_value(static_cast<unsigned int>(DRAW_EVERY_NODE_PERCENT)),
//         "set the draw mode")
//        ("rounds,r", po::value<unsigned int>(&totalRounds)->default_value(NUMBER_OF_ROUNDS),
//         "set the number of rounds");
        
        // This makes the input file a positional argument
        p.add("input-file", -1);
        po::store(po::command_line_parser(argc, argv).
                  options(desc).positional(p).run(), vm);
        po::notify(vm);
    }
    catch (const std::exception &exc)
    {
        // catch anything thrown within try block that derives from std::exception
        std::cerr << exc.what();
        return -1;
    }
    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        return 1;
    }
    if (vm.count("input-file"))
    {
        std::cout << "Input file: "
        << vm["input-file"].as<std::string>() << std::endl;
    }

    return EXIT_SUCCESS;
}
