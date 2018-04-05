#include "Util.hpp"

int getRandomInt(int i)
{
    std::random_device rd;   // non-deterministic generator  
    std::mt19937 gen(rd());  // to seed mersenne twister

    return gen() % i;
}

std::vector<std::string> splitString(std::string inString, char delimiter)
{
    std::vector<std::string> internal;
    std::stringstream ss(inString); // Turn the string into a stream.
    std::string temp;

    while (std::getline(ss, temp, delimiter))
    {
        internal.push_back(temp);
    }

    return internal;
}
