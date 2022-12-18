#include <algorithm>
#include <vector>

#include "../common/common.h"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <deque>
#include <limits>
#include <map>
#include <optional>
#include <utility>
#include <unordered_set>
#include <vector>

#include "../common/common.h"

//-----------------------------------------------------------------------------------

template<typename T>
struct Vec2 : std::pair<T, T> {
    using std::pair<T, T>::pair;

    virtual ~Vec2() = default;

    auto getX() const -> T  { return this->first; }
    auto getY() const -> T  { return this->second; }
    auto getX()       -> T& { return this->first; }
    auto getY()       -> T& { return this->second; }

    void setX(T x)          { this->first = x;  }
    void setY(T y)          { this->second = y; }

    auto getManhattanDist(const Vec2<T>& other) const -> T {
        auto diff = *this - other;
        return { std::abs(diff.getX()) + std::abs(diff.getY()) };
    }

    auto operator-(const Vec2<T>& other) const -> Vec2<T> {
        return { getX() - other.getX(), getY() - other.getY() };
    }
    friend auto operator<(const Vec2<T>& lhs, const Vec2<T>& rhs) -> bool {
        if (lhs.getY() != rhs.getY()) {
            return lhs.getY() < rhs.getY();
        }
        return lhs.getX() < rhs.getX();
    }
    friend auto operator<<(std::ostream& s, const Vec2<T>& v) -> std::ostream& {
        s << "X: " << v.getX() << " Y: " << v.getY();
        return s;
    }
};

using vec2_t = Vec2<int32_t>;

//-----------------------------------------------------------------------------------

struct CoordinateMap {
    std::map<vec2_t, char>  map;
    std::vector<vec2_t> intervals;
    std::vector<std::pair<vec2_t, vec2_t>> detections;
    int32_t targetY = 2000000;
    size_t nonBeacon = 0;
    int32_t minX = std::numeric_limits<int32_t>::max(), maxX = std::numeric_limits<int32_t>::min();

    virtual ~CoordinateMap() = default;

    void fill(const vec2_t& s, const vec2_t& b) {
        map[s] = 'S';
        map[b] = 'B';
        detections.push_back(std::pair<vec2_t, vec2_t>{ s, b });

        auto dist = b.getManhattanDist(s);
        int32_t yDiff = std::abs(s.getY() - targetY);
        if (yDiff < dist) {
            minX = std::min(minX, s.getX() - (dist - yDiff));
            maxX = std::max(maxX, s.getX() + (dist - yDiff));
        }
        common::print("BEACON: ", b, " SENSOR: ", s, " MANHATTAN DIST: ", dist);
        
        /* fillManhattan(s, dist); */
    }

    void fillManhattan(const vec2_t& p, int32_t d) { // maybe not use
        for (int32_t i = 0; i < d; ++i) {
            for (int32_t y = p.getY() - i; y <= p.getY() + i; ++y) {
                /* common::print("X: ", x, " Y: ", y, " (dist: ", p.getManhattanDist({ x, y }), ")"); */
                for (int32_t x = p.getX() - (d - i); x <= p.getX() + (d - i); ++x) {
                    if (isAvail({ x, y })) {
                        map[{x, y}] = '#';
                    }
                }
            }
        }
    }

    size_t findTarget(char target = '#') {
        size_t result = 0;
        for (const auto& [key, val] : map) {
            result += (key.getY() == targetY && val == target);
        }
        return result;
    }

    auto getMinKey() const -> vec2_t {
        vec2_t min{ map.cbegin()->first };
        for (const auto& [key, val] : map) {
            if (key.getX() < min.getX()) min.setX(key.getX());
            if (key.getY() < min.getY()) min.setY(key.getY());
        }
        return min;
    }

    auto getMaxKey() const -> vec2_t {
        vec2_t max{ 0, 0 };
        for (const auto& [key, val] : map) {
            if (key.getX() > max.getX()) max.setX(key.getX());
            if (key.getY() > max.getY()) max.setY(key.getY());
        }
        return max;
    }

    auto isAvail(const vec2_t& tile) const -> bool {
        const auto it = map.find(tile);
        return (it == map.cend());
    }

    auto isInBounds(const vec2_t& tile) const -> bool {
        auto min = getMinKey();
        auto max = getMaxKey();
        return (tile.getX() >= min.getX() && tile.getX() < max.getX() &&
                tile.getY() >= min.getY() && tile.getY() < max.getY());
    }

    friend auto operator<<(std::ostream& s, const CoordinateMap& map) -> std::ostream& {
        const auto min = map.getMinKey();
        const auto max = map.getMaxKey();
        for (int32_t y = min.getY(); y <= max.getY(); ++y) {
            std::string line{ std::to_string(y) };
            line += " ";
            for (int32_t x = min.getX(); x <= max.getX(); ++x) {
                const auto it = map.map.find({ x, y });
                if (it != map.map.cend()) {
                    line.push_back(it->second);
                } else {
                    line.push_back('.');
                }
            }
            s << line << "\n";
        }
        return s;
    }
};

//-----------------------------------------------------------------------------------

size_t solvePartOne(CoordinateMap coords) {
    /* common::print(""); */
    /* common::print(coords); */
    int beacons = 0;
    std::unordered_set<uint64_t> incl;
    for (const auto& [sensor, beacon] : coords.detections) {
        if (beacon.getY() == coords.targetY && beacon.getX() >= coords.minX && beacon.getX() <= coords.maxX) {
            uint64_t h = ((uint64_t)beacon.getX() << 32) | (uint64_t)beacon.getY();
            auto it = incl.find(h);
            if (it != incl.end()) {
                continue;
            }
            incl.insert(h);
            ++beacons;
        }
    }
    return (coords.maxX - coords.minX - beacons + 1);
}

size_t solvePartTwo(CoordinateMap coords) {
    size_t result = 0;
    // coords.addFloor();
    // while (coords.fillSand()) {
    //     /* common::print(coords); */
    //     ++result;
    // }
    return result + 1;
}

//-----------------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    common::assert(argc >= 2, "No inputfile provided");

    CoordinateMap coords;
    bool res = common::iterateFile(argv[1], [&coords](const std::string& line) {
            vec2_t sensor, beacon;
            std::sscanf(line.c_str(), "Sensor at x=%d, y=%d: closest beacon is at x=%d, y=%d",
                        &sensor.getX(), &sensor.getY(), &beacon.getX(), &beacon.getY());
            coords.fill(sensor, beacon);
        });
    common::assert(res, "Failed to iterate file");

    std::cout << "[Part I] Result: " << solvePartOne(coords) << "\n";
    std::cout << "[Part 2] Result: " << solvePartTwo(coords) << "\n";

    return 0;
}
