#include <fstream>
#include <functional>
#include <iostream>
#include <string>

inline bool iterateFile(const std::string& fileName, std::function<void(const std::string&)> func)
{
    std::ifstream input(fileName);
    if (!input.is_open()) {
        std::cerr << "Unable to open file '" << fileName << "'\n";
        return false;
    }

    std::string line;
    while (std::getline(input, line)) {
        func(line);
    }

    return true;
}
