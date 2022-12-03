#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

#include "../common/common.h"

struct Elf {
    std::vector<int> m_stash;

    void addItem(int i) {
        m_stash.push_back(i);
    }
    int getSum() const {
        return std::accumulate(m_stash.cbegin(), m_stash.cend(), 0);
    }
};

int main(int argc, char *argv[]) {
    common::assert(argc >= 2, "No inputfile provided");

    std::vector<Elf> elfs(1);
    bool res = common::iterateFile(argv[1], [&elfs](const std::string& line){
            if (line != "") {
                int cal = std::stoi(line);
                elfs.back().addItem(cal);
            } else {
                elfs.emplace_back(Elf());
            }
        });
    common::assert(res, "Failed to iterate file");

    std::vector<int> sums;
    for (const auto& elf : elfs) {
        sums.push_back(elf.getSum());
    }

    //================================ PART ONE ====================================
    const std::vector<int>::const_iterator max = std::max_element(sums.cbegin(), sums.cend());
    std::cout << "[Part I] Max element is: " << *max << "\n";

    //================================ PART TWO ====================================
    std::sort(sums.begin(), sums.end(), std::greater<int>());
    const int sumOfThree = std::accumulate(sums.cbegin(), sums.cbegin() + 3, 0);
    std::cout << "[Part 2] Sum of the first 3 elf cals: " << sumOfThree << "\n";

    return 0;
}
