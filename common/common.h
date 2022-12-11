#pragma once

#include <algorithm>
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

inline std::string strip(const std::string &s) {
    std::string result;
    result.reserve(s.length());
    std::remove_copy_if(s.begin(),
                        s.end(),
                        std::back_inserter(result),
                        std::not1(std::ptr_fun(isalnum)));
    return result;
}

inline std::string ltrim(std::string s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    return s;
}

template<typename ...Args>
inline void print(Args&& ...args) {
    std::ostringstream ss;
    (ss << ... << std::forward<Args>(args));
    std::cout << ss.str() << std::endl;
}

template<typename ...Args>
inline void assert(bool cond, Args&& ...args) {
    if (!cond) {
        print(std::forward<Args>(args)...);
    }
}

} // namespace common
