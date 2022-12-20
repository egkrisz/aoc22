#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <ranges>
#include <vector>

#include "../common/common.h"

//-----------------------------------------------------------------------------

using list_t = std::vector<int64_t>;

void printList(const list_t& list) {
    for (auto i : list) {
        std::cout << i << ", ";
    }
    std::cout << std::endl;
}

int64_t getValue(const list_t& list, const list_t& position, int64_t pos) {
    return list[std::find(position.cbegin(), position.cend(), pos % (position.size())) - position.cbegin()];
}

int64_t normalizePos(int64_t pos, int64_t len) {
    if (0 < pos && pos < len)
        return pos;
    if (pos <= 0)
        return pos + len - 1;
    return pos - len + 1;
}

void shufflePositions(const list_t& list, list_t& position) {
    const auto mod = static_cast<int64_t>(list.size() - 1);
    for (int64_t i = 0; i < mod + 1; ++i) {
        if (list[i] % mod == 0) continue;

        const auto oldPos = position[i];
        const auto newPos = normalizePos(oldPos + list[i] % (mod), list.size());
        for (auto &ele : position) {
            if (oldPos < newPos && oldPos <= ele && ele <= newPos) {
                ele = (ele + mod) % (list.size());
            } else if (newPos < oldPos && newPos <= ele && ele <= oldPos) {
                ele = (ele + 1) % list.size();
            }
        }
        position[i] = newPos;
    }
}

int64_t findCoordinates(const list_t& list, const list_t& position) {
    int64_t res = 0;
    const auto pos = std::find(list.cbegin(), list.cend(), 0) - list.cbegin();
    for (int64_t i = 1000; i <= 3000; i+=1000) {
        res += getValue(list, position, (i + position[pos]));
    }
    return res;
}

//-----------------------------------------------------------------------------

int64_t solvePartOne(const list_t& list) {
    list_t position;
    for (int64_t i = 0; i < list.size(); ++i) {
        position.push_back(i);
    }
    shufflePositions(list, position);
    return findCoordinates(list, position);
}

int64_t solvePartTwo(list_t list) {
    list_t position;
    for (int64_t i = 0; i < list.size(); ++i) {
        list[i] *= 811589153; // This magic constant is from the task desc...
        position.push_back(i);
    }
    for (int64_t _ = 0; _ < 10; ++_) {
        shufflePositions(list, position);
    }
    return findCoordinates(list, position);
}

//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    common::assert(argc >= 2, "No inputfile provided");

    list_t list;
    bool res = common::iterateFile(argv[1], [&list](const std::string& line){
        list.push_back(std::stoll(line.c_str()));
    });
    common::assert(res, "Failed to iterate file");

    std::cout << "[Part I] Result: " << solvePartOne(list) << "\n";
    std::cout << "[Part 2] Result: " << solvePartTwo(list) << "\n";

    return 0;
}
