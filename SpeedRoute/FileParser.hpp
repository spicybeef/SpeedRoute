//
//  FileParser.hpp
//  SpeedRoute
//
//  Created by Michel Kakulphimp on 2018-03-24.
//  Copyright © 2018 Michel Kakulphimp. All rights reserved.
//

#ifndef FileParser_hpp
#define FileParser_hpp

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "Types.hpp"
#include "Util.hpp"

class FileParser
{
public:
    FileParser(void);
    ~FileParser(void);
    
    void setFilenames(std::string netFilename, std::string placementFilename);
    bool parseInput(void);
    parsedInputStruct_t getParsedInput(void);

private:
    bool parseNetFile(void);
    bool parsePlacementFile(void);
    
    parsedInputStruct_t mParsedInput;
    std::string mNetFilename;
    std::string mPlacementFilename;
};

#endif /* FileParser_hpp */
