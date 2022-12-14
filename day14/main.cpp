#include <algorithm>
#include <vector>

#include "../common/common.h"

int main(int argc, char *argv[]) {
    common::assert(argc >= 2, "No inputfile provided");

    int resultI = 0; int resultII = 0;
    bool res = common::iterateFile(argv[1], [&resultI, &resultII](const std::string& line){
             common::print(line);
        });
    common::assert(res, "Failed to iterate file");

    std::cout << "[Part I] Result: " << resultI << "\n";
    std::cout << "[Part 2] Result: " << resultII << "\n";

    return 0;
}
