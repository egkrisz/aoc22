#pragma once

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
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

inline std::vector<std::string> tokenize(std::string_view v, std::string_view delim) {
    std::vector<std::string> result;

    size_t pos = 0;
    while ((pos = v.find(delim)) != std::string::npos) {
        result.emplace_back(v.substr(0, pos));
        v.remove_prefix(pos + delim.size());
    }
    result.emplace_back(v);
    return result;
}

inline std::vector<std::string> tokenize(const std::string& in, char delim = ' ') {
    return tokenize(std::string_view{ in.data(), in.size() }, std::string_view{ &delim, 1 });
}

inline std::string strip(const std::string &s) {
    std::string result;
    result.reserve(s.length());
    std::remove_copy_if(s.begin(), s.end(), std::back_inserter(result), std::not1(std::ptr_fun(isalnum)));
    return result;
}

inline std::string ltrim(std::string s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
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
