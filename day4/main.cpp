#include <algorithm>
#include <sstream>
#include <vector>

#include "../common/common.h"

struct Range {
    int begin; int end;

    bool contains(const Range& other) const {
        return (begin <= other.begin && end >= other.end);
    }
    bool overlaps(const Range& other) const {
        return (end >= other.begin && begin <= other.end);
    }
    Range(const std::string rangeStr) {
        std::vector<std::string> range = common::tokenize(rangeStr, '-');
        begin = std::stoi(range[0]);
        end = std::stoi(range[1]);
    }
};

struct Pair {
    Range first; Range second;

    Pair(const std::string& firstStr, const std::string& secondStr)
        : first(firstStr), second(secondStr) { }
    bool contains() const {
        return (first.contains(second) || second.contains(first));
    }
    bool overlaps() const {
        return (first.overlaps(second));
    }
};

int main(int argc, char *argv[]) {
    common::assert(argc >= 2, "No input file provided");

    int resultI = 0; int resultII = 0;
    bool res = common::iterateFile(argv[1], [&resultI, &resultII](const std::string& line){
            std::vector<std::string> pairs = common::tokenize(line, ',');
            Pair pair = Pair{ pairs[0], pairs[1] };
            if (pair.contains()) resultI++;
            if (pair.overlaps()) resultII++;
        });
    common::assert(res, "Failed to iterate file");

    std::cout << "[Part I] Result: " << resultI << "\n";
    std::cout << "[Part 2] Result: " << resultII << "\n";

    return 0;
}
