#include <algorithm>
#include <cstdio>
#include <deque>
#include <map>
#include <sstream>
#include <iterator>
#include <vector>

#include "../common/common.h"

struct Layout {
    std::map<int, std::deque<char>> stacks;

    Layout(const std::vector<std::string>& initLayout) {
        for (auto rit = initLayout.rbegin()+1; rit != initLayout.rend(); ++rit) {
            for (size_t i = 1; i < rit->size(); i+=4) {
                char c = rit->at(i);
                int stackId = (i + 3) / 4;
                if (std::isalpha(c)) {
                    stacks[stackId].push_back(c);
                }
            }
        }
    }

    void applyInstructions(const std::vector<std::string>& instructions, bool sameOrder = false) {
        for (auto& ins : instructions) {
            int count = 0, from = 0, to = 0;
            std::sscanf(ins.c_str(), "move %d from %d to %d", &count, &from, &to);
            if (!sameOrder) {
                executeReverseOrder(count, from, to);
            } else {
                executeSameOrder(count, from, to);
            }
        }
    }

    void executeReverseOrder(int count, int from, int to) { // For part one
        for (int i = 0; i < count; ++i) {
            char c = stacks[from].back();
            stacks[from].pop_back();
            stacks[to].push_back(c);
        }
    }

    void executeSameOrder(int count, int from, int to) { // For part two
        std::deque<char> cache;
        for (int i = 0; i < count; ++i) {
            cache.push_front(stacks[from].back());
            stacks[from].pop_back();
        }
        std::copy(cache.cbegin(), cache.cend(), std::back_inserter(stacks[to]));
    }

    std::string getTop() {
        std::stringstream ss;
        for (auto stack : stacks) {
            ss << stack.second.back();
        }
        return ss.str();
    }
};

std::string getResult(const std::vector<std::string>& initLayout, const std::vector<std::string>& instructions, bool sameOrder = false) {
    auto layout = Layout{ initLayout };
    layout.applyInstructions(instructions, sameOrder);
    return layout.getTop();
}

int main(int argc, char *argv[]) {
    common::assert(argc >= 2, "No inputfile provided");

    std::vector<std::string> initLayout, instructions;
    bool res = common::iterateFile(argv[1], [&initLayout, &instructions](const std::string& line){
            static bool initLayoutLoaded = false;
             if (line.empty()) {
                 initLayoutLoaded = true;
                 return;
             } else if (!initLayoutLoaded) {
                 initLayout.push_back(line);
             } else {
                 instructions.push_back(line);
             }
        });
    common::assert(res, "Failed to iterate file");

    std::cout << "[Part I] Result: " << getResult(initLayout, instructions) << "\n";
    std::cout << "[Part 2] Result: " << getResult(initLayout, instructions, true) << "\n";

    return 0;
}
