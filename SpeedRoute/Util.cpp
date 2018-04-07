#include "Util.hpp"

std::chrono::high_resolution_clock::time_point startTime;
std::chrono::high_resolution_clock::time_point endTime;

int Util_GetRandomInt(int i)
{
    std::random_device rd;   // non-deterministic generator  
    std::mt19937 gen(rd());  // to seed mersenne twister

    return gen() % i;
}

std::vector<std::string> Util_SplitString(std::string inString, char delimiter)
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

void Util_RecordStartTime(void)
{
    startTime = std::chrono::high_resolution_clock::now();
}

double Util_GetElapsedTime(void)
{
    std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = currentTime - startTime;
    return elapsed.count();
}

void Util_RecordFinishTime(void)
{
    endTime = std::chrono::high_resolution_clock::now();
}

double Util_GetFinishTime(void)
{
    std::chrono::duration<double> elapsed = endTime - startTime;
    return elapsed.count();
}
