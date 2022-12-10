#include <algorithm>
#include <sstream>
#include <vector>

#include "../common/common.h"

// It is better to store instructions represented as their effect to register X in every cycle.
// This means noop is stored as a single 0 and addx is two instructions; a noop and the change to regX.
using instruction_t = int32_t;

size_t solvePartOne(const std::vector<instruction_t>& instructions) {
    int32_t regX = 1;
    size_t signalStrength = 0;
    for (size_t i = 0; i < instructions.size(); ++i) {
        auto cycle = i + 1;
        if (cycle % 40 == 20) { // this checks for all the interesting cycles
            signalStrength += (cycle * regX);
        }
        regX += instructions[i]; // no additional checks are needed since noop is 0 now
    }
    return signalStrength;
}

std::string solvePartTwo(const std::vector<instruction_t>& instructions) {
    int32_t regX = 1;
    std::stringstream ss;
    auto line = std::string(40, '.');
    for (size_t i = 0; i < instructions.size(); ++i) {
        int32_t px = i % 40;
        if (px <= regX + 1 && px >= regX - 1) {
            line[px] = '#';
        }
        if ((i + 1) % 40 == 0) { // create a blank line after every 40 px row
            ss << line << '\n';
            line = std::string(40, '.');
        }
        regX += instructions[i];
    }
    return ss.str();
}

int main(int argc, char *argv[]) {
    common::assert(argc >= 2, "No inputfile provided");

    std::vector<instruction_t> instructions;
    bool res = common::iterateFile(argv[1], [&instructions](const std::string& line){
             auto toks = common::tokenize(line);
             instructions.push_back(0);
             if (toks.size() == 2) {
                 instructions.push_back(std::stoi(toks[1]));
             }
        });
    common::assert(res, "Failed to iterate file");

    std::cout << "[Part I] Result: " << solvePartOne(instructions) << "\n";
    std::cout << "[Part 2] Result: " << "\n";
    std::cout << solvePartTwo(instructions);

    return 0;
}
