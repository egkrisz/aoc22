#include <algorithm>
#include <iterator>
#include <vector>

#include "../common/common.h"

template<typename NumLimit, typename Func, typename FuncParam>
bool iterate(NumLimit init, NumLimit term, size_t other, int curr, Func func, FuncParam& param, bool isIncrementing = true) {
    auto compOp = (isIncrementing) ? [](NumLimit first, NumLimit second) { return first < second; }
                                   : [](NumLimit first, NumLimit second) { return first >= second; };
    auto incrOp = (isIncrementing) ? [](NumLimit& value) { ++value; }
                                   : [](NumLimit& value) { --value; };
    for (NumLimit i = init; compOp(i, term); incrOp(i)) {
        if (func(curr, i, other, param)) {
            return true;
        }
    }
    return false;
}

struct Grid {
    std::vector<std::vector<int>> elements;

    void populateRow(std::vector<int>&& row) {
        elements.push_back(row);
    }
    size_t getVisibility() {
        size_t visibility = (elements.size()) * 2 + (elements[0].size() - 2) * 2;
        auto rowComparator = [this](int curr, size_t var, size_t other, size_t& visibility) { return (elements[var][other] >= curr); };
        auto colComparator = [this](int curr, size_t var, size_t other, size_t& visibility) { return (elements[other][var] >= curr); };

        for (size_t r = 1; r < elements.size() - 1; ++r) {
            for (size_t c = 1; c < elements[r].size() - 1; ++c) {
                int curr = elements[r][c];
                if (!iterate(r+1, elements.size(), c, curr, rowComparator, visibility, true)) {
                    visibility++;
                    continue;
                }
                if (!iterate(static_cast<int>(r-1), 0, c, curr, rowComparator, visibility, false)) {
                    visibility++;
                    continue;
                }
                if (!iterate(c+1, elements[r].size(), r, curr, colComparator, visibility, true)) {
                    visibility++;
                    continue;
                }
                if (!iterate(static_cast<int>(c-1), 0, r, curr, colComparator, visibility, false)) {
                    visibility++;
                    continue;
                }
            }
        }
        return visibility;
    }
    size_t getScenicScore() {
        size_t scenicScore = 0;
        auto rowComparator = [this](int curr, size_t var, size_t other, size_t& score) { score++; return (elements[var][other] >= curr); };
        auto colComparator = [this](int curr, size_t var, size_t other, size_t& score) { score++; return (elements[other][var] >= curr); };

        for (size_t r = 1; r < elements.size() - 1; ++r) {
            for (size_t c = 1; c < elements[r].size() - 1; ++c) {
                int curr = elements[r][c];

                size_t scoreUp = 0, scoreDown = 0, scoreRight = 0, scoreLeft = 0;
                iterate(r+1, elements.size(), c, curr, rowComparator, scoreUp, true);
                iterate(static_cast<int>(r-1), 0, c, curr, rowComparator, scoreDown, false);
                iterate(c+1, elements[r].size(), r, curr, colComparator, scoreRight, true);
                iterate(static_cast<int>(c-1), 0, r, curr, colComparator, scoreLeft, false);

                size_t currScenicScore = scoreUp * scoreDown * scoreRight * scoreLeft;
                if (currScenicScore > scenicScore)
                    scenicScore = currScenicScore;
            }
        }
        return scenicScore;
    }
};

int main(int argc, char *argv[]) {
    common::assert(argc >= 2, "No inputfile provided");

    Grid grid{ };
    bool res = common::iterateFile(argv[1], [&grid](const std::string& line){
            std::vector<int> row;
            std::transform(line.cbegin(), line.cend(), std::back_inserter(row), [](const char c) { return std::stoi(std::string{ c }); });
            grid.populateRow(std::move(row));
        });
    common::assert(res, "Failed to iterate file");

    std::cout << "[Part I] Result: " << grid.getVisibility() << "\n";
    std::cout << "[Part 2] Result: " << grid.getScenicScore() << "\n";

    return 0;
}
