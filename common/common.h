#pragma once

#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <sstream>
#include <utility>
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

inline std::vector<std::string> tokenize(const std::string& in, char delim = ' ')
{
    std::vector<std::string> out;
    std::stringstream ss(in);
    std::string tok;
    while (!ss.eof()) {
        std::getline(ss, tok, delim);
        out.emplace_back(tok);
    }
    return out;
}

template<typename ...Args>
inline void assert(bool cond, Args&& ...args) {
    if (!cond) {
        std::ostringstream ss;
        (ss << ... << std::forward<Args>(args));
        std::cerr << ss.str() << std::endl;
        abort();
    }
}

} // namespace common
