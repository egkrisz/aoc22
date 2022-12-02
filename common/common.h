#pragma once

#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

namespace common {

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

inline void tokenize(const std::string& in, std::vector<std::string>& out, char delim)
{
    std::stringstream ss(in);
    std::string tok;
    while (!ss.eof()) {
        std::getline(ss, tok, delim);
        out.emplace_back(tok);
    }
}

} // namespace common
