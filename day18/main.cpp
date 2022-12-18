#include <algorithm>
#include <cstdint>
#include <map>
#include <set>
#include <vector>

#include "../common/common.h"

//-----------------------------------------------------------------------------

struct Vec3 {
    int64_t x = 0, y = 0, z = 0;

    auto operator==(const Vec3& other) const -> bool {
        return x == other.x && y == other.y && z == other.z;
    }
    auto operator<=(const Vec3& other) const -> bool {
        return x <= other.x && y <= other.y && z <= other.z;
    }
    auto operator>=(const Vec3& other) const -> bool {
        return x >= other.x && y >= other.y && z >= other.z;
    }
    auto operator+(const Vec3& other) const -> Vec3 {
        return { x + other.x, y + other.y, z + other.z };
    }
    auto operator-(const Vec3& other) const -> Vec3 {
        return { x - other.x, y - other.y, z - other.z };
    }
    auto operator+(int64_t val) const -> Vec3 {
        return { x + val, y + val, z + val };
    }
    auto operator-(int64_t val) const -> Vec3 {
        return { x - val, y - val, z - val };
    }
    friend auto operator<<(std::ostream& s, const Vec3& pos) -> std::ostream& {
        s << "X: " << pos.x << " Y: " << pos.y << " Z: " << pos.z;
        return s;
    }
};

auto operator<(const Vec3& lhs, const Vec3 rhs) {
    if (lhs.x != rhs.x)
        return lhs.x < rhs.x;
    else if (lhs.y != rhs.y)
        return lhs.y < rhs.y;
    else
        return lhs.z < rhs.z;
};

using counter_t = std::map<int64_t, int64_t>;

//-----------------------------------------------------------------------------

constexpr std::array<Vec3, 6> adjacencies = { {
        { 0,1,0  }, // north
        { 1,0,0  }, // east
        { 0,-1,0 }, // west
        { -1,0,0 }, // south
        { 0,0,1  }, // up
        { 0,0,-1 }, // down
    } };

//-----------------------------------------------------------------------------

int64_t solvePartOne(const std::vector<Vec3>& positions) {
    int64_t result = 0;
    std::set<Vec3> s;
    std::copy(positions.cbegin(), positions.cend(), std::inserter(s, s.begin()));

    for (const auto& pos : positions) {
        for (const auto& adj : adjacencies) {
            if (s.find(pos + adj) == s.end()) {
                result += 1;
            }
        }
    }
    return result;
}

int64_t solvePartTwo(const std::vector<Vec3>& positions) {
    int64_t result = 0;
    std::set<Vec3> s, ext;
    std::copy(positions.cbegin(), positions.cend(), std::inserter(s, s.begin()));

    Vec3 min = { INT64_MAX, INT64_MAX, INT64_MAX }, max{ 0, 0, 0 };
    for (const auto& pos : positions) {
        min.x = std::min(min.x, pos.x); max.x = std::max(max.x, pos.x);
        min.y = std::min(min.y, pos.y); max.y = std::max(max.y, pos.y);
        min.z = std::min(min.z, pos.z); max.z = std::max(max.z, pos.z);
    }
    std::function<void(const Vec3&)> fillExt = [&ext, &min, &max, &s, &fillExt](const Vec3& v) {
        if (v >= min - 1 && v <= max + 1 &&
            s.find(v) == s.end() && ext.find(v) == ext.end()) {
            ext.insert(v);
            for (const auto& adj : adjacencies) {
                fillExt(v + adj);
            }
        }
    };
    fillExt({ min - 1 });

    for (const auto& pos : positions) {
        for (const auto& adj : adjacencies) {
            if (ext.find(pos + adj) != ext.end()) {
                result += 1;
            }
        }
    }
    return result;
}

//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    common::assert(argc >= 2, "No inputfile provided");

    std::vector<Vec3> positions;
    bool res = common::iterateFile(argv[1], [&positions](const std::string& line){
        auto toks = common::tokenize(line, ",");
        common::assert(toks.size() == 3, "Invalid input format");
        positions.push_back({ std::stoll(toks[0].c_str()), std::stoll(toks[1].c_str()), std::stoll(toks[2].c_str()) });
    });
    common::assert(res, "Failed to iterate file");

    std::cout << "[Part I] Result: " << solvePartOne(positions) << "\n";
    std::cout << "[Part 2] Result: " << solvePartTwo(positions) << "\n";

    return 0;
}
