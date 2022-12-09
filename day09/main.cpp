#include <algorithm>
#include <array>
#include <cmath>
#include <map>
#include <unordered_set>
#include <sstream>

#include "../common/common.h"

int32_t getSign(int32_t x) {
    if      (x < 0)  return -1;
    else if (x > 0)  return 1;
    return 0; // if x == 0
}

struct Pos {
    int32_t x = 0, y = 0;

    bool operator==(const Pos& other) const {
        return (x == other.x && y == other.y);
    }
    bool operator!=(const Pos& other) const {
        return !(*this == other);
    }
    Pos& operator=(const Pos& other) {
        if (*this != other) {
            x = other.x;
            y = other.y;
        }
        return *this;
    }
    Pos operator+(const Pos& other) const {
        return { x + other.x, y + other.y };
    }
    Pos operator-(const Pos& other) const {
        return { x - other.x, y - other.y };
    }
    Pos& operator+=(const Pos& other) {
        *this = *this + other;
        return *this;
    }
    Pos& operator<<(const Pos& other) {
        x += getSign(other.x);
        y += getSign(other.y);
        return *this;
    }
};

struct PosHash {
    size_t operator()(const Pos& p) const {
        return std::hash<int32_t>{}(p.x) ^ std::hash<int32_t>{}(p.y);
    }
};

static const std::map<char, Pos> moves = {
    { 'U', { 0, 1 } }, { 'D', { 0, -1 } },
    { 'R', { 1, 0 } }, { 'L', { -1, 0 } }
};

template<int N>
struct Rope {
    std::array<Pos, N> ropes;
    std::unordered_set<Pos, PosHash> visited { Pos{ } };

    size_t getVisited() const {
        return visited.size();
    }
    void moveHead(char direction, size_t steps) {
        for (size_t i = 0; i < steps; ++i) {
            ropes[0] += moves.at(direction);

            for (size_t r = 0; r < ropes.size() - 1; ++r) {
                Pos dist = ropes[r] - ropes[r+1];
                if (std::abs(dist.x) > 1 || std::abs(dist.y) > 1) {
                    ropes[r+1] << dist;
                }
            }
            visited.insert(ropes[ropes.size()-1]);
        }
    }
};

int main(int argc, char *argv[]) {
    common::assert(argc >= 2, "No inputfile provided");

    Rope<2>  rope{ };
    Rope<10> brokenRope{ };
    bool res = common::iterateFile(argv[1], [&rope, &brokenRope](const std::string& line){
             auto tok = common::tokenize(line, ' ');
             rope.moveHead(tok[0][0], std::stoi(tok[1]));
             brokenRope.moveHead(tok[0][0], std::stoi(tok[1]));
        });
    common::assert(res, "Failed to iterate file");

    std::cout << "[Part I] Result: " << rope.getVisited() << "\n";
    std::cout << "[Part 2] Result: " << brokenRope.getVisited() << "\n";

    return 0;
}
