#include <algorithm>
#include <set>
#include <vector>

#include "../common/common.h"

int findMarker(const std::string& buffer, size_t markerLen) {
    for (size_t i = markerLen - 1; i < buffer.size(); ++i) {
        std::set<char> temp{ buffer.begin() + i - (markerLen - 1), buffer.begin() + i + 1 };
        if (temp.size() == markerLen) return i+1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    common::assert(argc >= 2, "No inputfile provided");

    std::string buffer;
    bool res = common::iterateFile(argv[1], [&buffer](const std::string& line){ buffer = line; });
    common::assert(res, "Failed to iterate file");

    std::cout << "[Part I] Result: " << findMarker(buffer, 4) << "\n";
    std::cout << "[Part 2] Result: " << findMarker(buffer, 14) << "\n";

    return 0;
}
