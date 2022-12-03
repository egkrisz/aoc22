#include <algorithm>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <map>
#include <set>
#include <vector>

#include "../common/common.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "No inputfile provided\n";
        abort();
    }

    uint64_t scoreI = 0;
    bool res = common::iterateFile(argv[1], [&scoreI](const std::string& line){
            size_t len = line.length();
            std::string comp1 = line.substr(0, len / 2);
            std::string comp2 = line.substr(len / 2);

            std::sort(comp1.begin(), comp1.end());
            std::sort(comp2.begin(), comp2.end());

            std::string intersect;
            std::set_intersection(comp1.cbegin(), comp1.cend(), comp2.cbegin(), comp2.cend(), std::back_inserter(intersect));

            char common = intersect[0];
            int prio = (islower(common)) ? (common - 'a') + 1 : (common - 'A') + 27;

            scoreI += prio;

            std::cout << line << " len: " << len << " comp1: " << comp1 << " comp2: " << comp2 << " common: " << common << " prio: " << prio << '\n';
        });

    if (!res) {
        abort();
    }

    uint64_t scoreII = 0;
    std::vector<std::string> backpacks;
    res = common::iterateFile(argv[1], [&backpacks](const std::string& line){
            backpacks.push_back(line);
        });
    for (size_t i = 0; i < backpacks.size() - 2; i+=3) {
        std::string bp1 = backpacks[i];
        std::string bp2 = backpacks[i+1];
        std::string bp3 = backpacks[i+2];
        
        std::sort(bp1.begin(), bp1.end());
        std::sort(bp2.begin(), bp2.end());
        std::sort(bp3.begin(), bp3.end());

        std::string intersect;
        std::string res;
        std::set_intersection(bp1.cbegin(), bp1.cend(), bp2.cbegin(), bp2.cend(), std::back_inserter(intersect));
        std::set_intersection(intersect.cbegin(), intersect.cend(), bp3.begin(), bp3.end(), std::back_inserter(res));

        char common = res[0];
        int prio = (islower(common)) ? (common - 'a') + 1 : (common - 'A') + 27;
        scoreII += prio;

        std::cout << "P2 Intersect: " << res << " prio: " << prio << '\n';
    }

    if (!res) {
        abort();
    }

    //================================ PART ONE ====================================
    std::cout << "[Part I] Total score: " << scoreI << '\n';

    //================================ PART TWO ====================================
    std::cout << "[Part II] Total score: " << scoreII << '\n';

    return 0;
}
