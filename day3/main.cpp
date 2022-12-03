#include <algorithm>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <numeric>
#include <map>
#include <set>
#include <vector>

#include "../common/common.h"

int getPrio(char val) {
    return (islower(val)) ? (val - 'a') + 1 : (val - 'A') + 27;
}

std::string getIntersection(std::string s1, std::string s2) {
    std::sort(s1.begin(), s1.end());
    std::sort(s2.begin(), s2.end());

    std::string intersect;
    std::set_intersection(s1.cbegin(), s1.cend(), s2.cbegin(), s2.cend(), std::back_inserter(intersect));
    return intersect;
}

int solvePartOne(const std::vector<std::string>& backpacks) {
    int score = 0;
    for (const auto& line : backpacks) {
        std::string res = getIntersection(line.substr(0, line.size() / 2), line.substr(line.size() / 2));
        score += getPrio(res[0]);
    }
    return score;
}

int solvePartTwo(const std::vector<std::string>& backpacks) {
    int score = 0;
    for (size_t i = 0; i < backpacks.size() - 2; i+=3) {
        std::string res = getIntersection(getIntersection(backpacks[i], backpacks[i+1]), backpacks[i+2]);
        score += getPrio(res[0]);
    }
    return score;
}

int main(int argc, char *argv[]) {
    common::assert(argc >= 2, "No inputfile provided");

    std::vector<std::string> backpacks;
    bool res = common::iterateFile(argv[1], [&backpacks](const std::string& line){ backpacks.push_back(line); });
    common::assert(res, "Failed to iterate through file");

    std::cout << "[Part I] Total score: " << solvePartOne(backpacks) << '\n';
    std::cout << "[Part II] Total score: " << solvePartTwo(backpacks) << '\n';

    return 0;
}
