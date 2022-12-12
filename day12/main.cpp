#include <algorithm>
#include <array>
#include <cstdint>
#include <deque>
#include <sstream>
#include <optional>
#include <vector>

#include "../common/common.h"

template<typename T>
struct Pos { // lifted from day09 -- maybe move to common???
    T x = 0, y = 0;

    bool operator==(const Pos<T>& other) const {
        return (x == other.x && y == other.y);
    }
    bool operator!=(const Pos<T>& other) const {
        return !(*this == other);
    }
    Pos& operator=(const Pos<T>& other) {
        if (*this != other) {
            x = other.x;
            y = other.y;
        }
        return *this;
    }
    Pos<T> operator+(const Pos<T>& other) const {
        return { x + other.x, y + other.y };
    }
    Pos<T> operator-(const Pos<T>& other) const {
        return { x - other.x, y - other.y };
    }
    Pos<T>& operator+=(const Pos<T>& other) {
        *this = *this + other;
        return *this;
    }
    void print() const {
        common::print("X: ", x, ", Y: ", y);
    }
};

using pos_t = Pos<int64_t>;
using dist_t = std::pair<int64_t, pos_t>;

template<typename T>
struct Grid {
    std::vector<std::vector<T>> data;

    Grid() = default;
    Grid(size_t x, size_t y) {
        resize(x, y);
    }
    T operator[](const pos_t& pos) const {
        return data[pos.x][pos.y];
    }
    T& operator[](const pos_t& pos) {
        return data[pos.x][pos.y];
    }
    T get(const pos_t& pos) const {
        return data[pos.x][pos.y];
    }
    size_t rows() const {
        return data.size();
    }
    size_t cols() const {
        return data[0].size();
    }
    bool isValid(const pos_t& pos) const {
        return isValid(pos.x, pos.y);
    }
    bool isValid(size_t x, size_t y) const {
        if ((x >= 0 && x < rows()) && (y >= 0 && y < cols())) {
            return true;
        }
        return false;
    }
    void resize(size_t x, size_t y) {
        data.resize(x);
        for (auto& v : data) {
            v.resize(y);
        }
    }
    void insert(const pos_t& pos, const T& value) {
        if (isValid(pos)) {
            data[pos.x][pos.y] = value;
        }
    }
    void push_back(const std::vector<T>& row) {
        data.push_back(row);
    }
    pos_t find(const T& val) const {
        for (size_t x = 0; x < rows(); ++x) {
            for (size_t y = 0; y < cols(); ++y) {
                if (data[x][y] == val) {
                    return { (int64_t)x, (int64_t)y };
                }
            }
        }
        return { };
    }
};

using grid_t = Grid<char>;

size_t findShortestPath(grid_t grid, char startingPos = 'S') {
    const auto endPos = grid.find('E');
    grid.insert(endPos, 'z');

    Grid<bool> visited{ grid.rows(), grid.cols() };
    visited.insert(endPos, true);

    std::deque<dist_t> dists { { 0, endPos } }; // this stores the distance from E for every position
    while (!dists.empty()) {
        auto [dist, pos] = dists.front();
        dists.pop_front();

        if (grid[pos] == startingPos) {
            return dist;
        }

        static const std::array<Pos<int64_t>, 4> dirs = { {
                { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } // up, down, right, left
            } };

        for (auto dir : dirs) {
            const auto np = pos + dir;
            if (visited.isValid(np) && !visited.get(np)) {             // If this tile was not visited yet,
                const auto prev = (grid[np] == 'S') ? 'a' : grid[np];  // store its position and check
                if (grid.get(pos) - prev <= 1) {                       // if characters are consecutive.
                    visited.insert(np, true);                          // If yes, insert as visited and store
                    dists.emplace_back(dist + 1, np);                  // as the last step in the queue.
                }
            }
        }
    }
    return 0;
}

size_t solvePartOne(const grid_t& grid) {
    return findShortestPath(grid, 'S');
}

size_t solvePartTwo(const grid_t& grid) {
    return findShortestPath(grid, 'a');
}

int main(int argc, char *argv[]) {
    common::assert(argc >= 2, "No inputfile provided");

    grid_t grid;
    bool res = common::iterateFile(argv[1], [&grid](const std::string& line){
            grid.push_back({ line.cbegin(), line.cend() });
        });
    common::assert(res, "Failed to iterate file");

    std::cout << "[Part I] Result: " << solvePartOne(grid) << "\n";
    std::cout << "[Part 2] Result: " << solvePartTwo(grid) << "\n";

    return 0;
}
