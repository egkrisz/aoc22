#include <algorithm>
#include <deque>
#include <map>
#include <optional>
#include <utility>
#include <vector>

#include "../common/common.h"

//-----------------------------------------------------------------------------------

struct Vec2 : std::pair<size_t, size_t> {
    using pair::pair;

    virtual ~Vec2() = default;

    auto getX() const -> size_t { return first; }
    auto getY() const -> size_t { return second; }
    void setX(size_t x)         { first = x;  }
    void setY(size_t y)         { second = y; }

    auto isInlineX(const Vec2& other) const -> bool { return getX() == other.getX(); }
    auto isInlineY(const Vec2& other) const -> bool { return getY() == other.getY(); }

    auto up()        const -> Vec2 { return { getX(), getY() - 1 }; }
    auto down()      const -> Vec2 { return { getX(), getY() + 1 }; }
    auto leftDown()  const -> Vec2 { return { getX() - 1, getY() + 1 }; }
    auto rightDown() const -> Vec2 { return { getX() + 1, getY() + 1 }; }

    friend auto operator<(const Vec2& lhs, const Vec2& rhs) -> bool {
        if (lhs.getY() != rhs.getY()) {
            return lhs.getY() < rhs.getY();
        }
        return lhs.getX() < rhs.getX();
    }
    friend auto operator<<(std::ostream& s, const Vec2& v) -> std::ostream& {
        s << "X: " << v.getX() << " Y: " << v.getY();
        return s;
    }
};

//-----------------------------------------------------------------------------------

struct CoordinateMap {
    std::map<Vec2, char>  map;
    std::optional<size_t> floor{ std::nullopt };

    virtual ~CoordinateMap() = default;

    void fill(const Vec2& from, const Vec2& to, char value) {
        if (from.isInlineX(to)) {
            auto min = std::min(from.getY(), to.getY());
            auto max = std::max(from.getY(), to.getY());
            for (size_t i = min; i <= max; ++i) {
                map[Vec2{ from.getX(), i }] = value;
            }
        } else if (from.isInlineY(to)) {
            auto min = std::min(from.getX(), to.getX());
            auto max = std::max(from.getX(), to.getX());
            for (size_t i = min; i <= max; ++i) {
                map[Vec2{ i, from.getY() }] = value;
            }
        } else {
            common::assert(0, "At least one of two values has to be inline in order to use 'fillMap()");
        }
    }

    void addFloor() { // For part two
        auto max = getMaxKey();
        floor = max.getY() + 2;
    }

    auto fillSand() -> bool {
        auto firstFree = findFirstAvail(500);
        if (!firstFree) {
            return false;
        }
        auto tile = *firstFree;
        while (true) {
            if (!isInBounds(tile)) {
                return false;
            }

            if (isAvail(tile.down())) {
                tile = tile.down();
            } else if (isAvail(tile.leftDown())) { // X - 1 and Y - 1
                tile = tile.leftDown();
            } else if (isAvail(tile.rightDown())) { // X + 1 and Y - 1
                tile = tile.rightDown();
            } else {
                break;
            }
        }
        if (tile.getY() == 0) { // For part two
            return false;
        }
        map[tile] = 'o';
        return true;
    }

    auto getMinKey() const -> Vec2 {
        Vec2 min{ map.cbegin()->first };
        for (const auto& [key, val] : map) {
            if (key.getX() < min.getX()) min.setX(key.getX());
            if (key.getY() < min.getY()) min.setY(key.getY());
        }
        return min;
    }

    auto getMaxKey() const -> Vec2 {
        Vec2 max{ 0, 0 };
        for (const auto& [key, val] : map) {
            if (key.getX() > max.getX()) max.setX(key.getX());
            if (key.getY() > max.getY()) max.setY(key.getY());
        }
        return max;
    }

    auto findFirstAvail(size_t x) const -> std::optional<Vec2> {
        for (const auto& [key, val] : map) {
            if (key.getY() > 0 && key.getX() == x && (!floor || key.getY() < *floor)) {
                return { key.up() };
            }
        }
        return {};
    }

    auto isAvail(const Vec2& tile) const -> bool {
        const auto it = map.find(tile);
        return (it == map.cend() && (!floor || tile.getY() < *floor));
    }

    auto isInBounds(const Vec2& tile) const -> bool {
        if (floor) { // For part two
            return true;
        }
        
        auto min = getMinKey();
        auto max = getMaxKey();
        return (tile.getX() >= min.getX() && tile.getX() < max.getX() &&
                tile.getY() >= min.getY() && tile.getY() < max.getY());
    }

    friend auto operator<<(std::ostream& s, const CoordinateMap& map) -> std::ostream& {
        const auto min = map.getMinKey();
        const auto max = map.getMaxKey();
        for (size_t y = min.getY(); y <= max.getY(); ++y) {
            std::string line{ std::to_string(y) };
            line += " ";
            for (size_t x = min.getX(); x <= max.getX(); ++x) {
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

Vec2 readCoords(std::string_view v) {
    size_t pos = v.find(',');
    auto x = v.substr(0, pos);
    v.remove_prefix(pos + 1);

    return { std::stoull(x.data()), std::stoull(v.data()) };
}

//-----------------------------------------------------------------------------------

size_t solvePartOne(CoordinateMap coords) {
    size_t result = 0;
    while (coords.fillSand()) {
        /* common::print(coords); */
        ++result;
    }
    return result;
}

size_t solvePartTwo(CoordinateMap coords) {
    size_t result = 0;
    coords.addFloor();
    while (coords.fillSand()) {
        /* common::print(coords); */
        ++result;
    }
    return result + 1;
}

//-----------------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    common::assert(argc >= 2, "No inputfile provided");

    CoordinateMap coords;
    coords.fill({ 500, 0 }, { 500, 0 }, '+');
    bool res = common::iterateFile(argv[1], [&coords](const std::string& line) {
            auto toks = common::tokenize(line, " -> ");
            for (size_t i = 0; i < toks.size() - 1; ++i) {
                coords.fill(readCoords(toks[i]), readCoords(toks[i+1]), '#');
            }
        });
    common::assert(res, "Failed to iterate file");

    std::cout << "[Part I] Result: " << solvePartOne(coords) << "\n";
    std::cout << "[Part 2] Result: " << solvePartTwo(coords) << "\n";

    return 0;
}
