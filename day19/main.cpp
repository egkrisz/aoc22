#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <vector>

#include "../common/common.h"

//-----------------------------------------------------------------------------

enum class Material : int64_t { Ore = 0, Clay = 1, Obsidian = 2, Geode = 3, None = -1 };

auto toString(Material m) -> std::string {
    switch (m) {
        case Material::Ore:      return "ore";
        case Material::Clay:     return "clay";
        case Material::Obsidian: return "obsidian";
        case Material::Geode:    return "geode";
        case Material::None:     return "none";
    }
    return "none";
}

auto index(Material m)   -> size_t   { return static_cast<size_t>(m); }
auto material(int64_t m) -> Material { return static_cast<Material>(m); }

//-----------------------------------------------------------------------------

struct Robot {
    Material               type{ Material::None };
    std::array<int64_t, 4> cost{ 0, 0, 0, 0 };

    friend auto operator<<(std::ostream& os, const Robot& r) -> std::ostream& {
        os << "Each " << toString(r.type) << " robot costs ";
        os << r.cost[0] << " ore";
        for (int64_t i = 1; i < 4; ++i) {
            if (r.cost[i] != 0) {
                os << " and " << r.cost[i] << " " << toString(material(i));
            }
        }
        return os;
    }
};

struct Blueprint {
    int64_t              id{ 0 };
    std::array<Robot, 4> robots;

    friend auto operator<<(std::ostream& os, const Blueprint& bp) -> std::ostream& {
        os << "Blueprint " << bp.id << ": ";
        for (const auto& r : bp.robots) {
            os << r << ". ";
        }
        return os;
    }
};

//-----------------------------------------------------------------------------

Blueprint makeBlueprint(std::string_view str) {
    Blueprint bp{ };

    // Get the id of the blueprint
    std::sscanf(str.data(), "Blueprint %ld", &bp.id);
    auto pos = str.find(":");
    str.remove_prefix(pos + 2);

    // Get the individual robots
    auto toks = common::tokenize(str, ". ");
    for (int64_t i = 0; i < 4; ++i) {
        // Assume that blueprint is in same order in all lines
        bp.robots[i].type = static_cast<Material>(i);

        // Get relevant part
        std::string_view tok = toks[i];
        pos = tok.find("costs");
        tok.remove_prefix(pos + 6);

        // Parse the count (the first count should always be Ore)
        bp.robots[i].cost[index(Material::Ore)] = std::stoll(std::string{ tok.substr(0, tok.find(" ")) }.c_str());

        pos = tok.find(" and "); // Look for a second part
        if (pos != tok.npos) {
            tok.remove_prefix(pos + 5);
            auto idx = (i <= 2) ? index(Material::Clay) : index(Material::Obsidian);
            auto cnt = std::stoll(std::string{ tok.substr(0, tok.find(" ")) }.c_str());
            bp.robots[i].cost[idx] = cnt;
        }
    }
    return bp;
}

struct State {
    Blueprint blueprint;
    std::array<int64_t, 4> stash { 0, 0, 0, 0 };
    std::array<int64_t, 4> robots{ 1, 0, 0, 0 };

    void growStash(int64_t time) {
        for (int64_t i = 0; i < 4; ++i) {
            stash[i] += robots[i] * time;
        }
    }

    void buildRobot(Material m) {
        robots[index(m)] += 1;
        for (int64_t i = 0; i < 4; ++i) {
            stash[i] -= blueprint.robots[index(m)].cost[i];
        }
    }

    auto estimateTime(Material robotType) -> int64_t {
        auto eta   = [](uint64_t needed, uint64_t produced) -> int64_t { return (needed + produced - 1) / produced; };

        int64_t dt = 0;
        const auto& cost = blueprint.robots[index(robotType)].cost;
        for (int64_t i = 0; i < 3; ++i) {
            if (cost[i] == 0 || stash[i] > cost[i]) continue;

            dt = std::max(dt, eta(cost[i] - stash[i], robots[i]));
        }
        return dt + 1;
    }

    auto canBuild(Material robotType) -> bool {
        auto cost  = [this](Material robot, Material price) -> int64_t { return blueprint.robots[index(robot)].cost[index(price)]; };
        auto robot = [this](Material m) -> int64_t { return robots[index(m)]; };

        switch (robotType) {
            case Material::Geode:
            case Material::Obsidian:
                return (robots[index(robotType)-1]);
            case Material::Clay:
                return robot(Material::Clay) < cost(Material::Obsidian, Material::Clay);
            case Material::Ore:
                return robot(Material::Ore) < cost(Material::Clay, Material::Ore) ||
                       robot(Material::Ore) < cost(Material::Obsidian, Material::Ore) ||
                       robot(Material::Ore) < cost(Material::Geode, Material::Ore);
            default: return false;
        }
    }

    static auto simulate(int64_t time, State state) -> int64_t {
        int64_t best = state.stash[index(Material::Geode)] + time * state.robots[index(Material::Geode)]; // Score when nothing is done

        for (int64_t i = 0; i < 4; ++i) { // Try to build each robot and simulate what happens in each case (recursion alert)
            if (state.canBuild(material(i))) {
                const auto dt = state.estimateTime(material(i));
                if (time > dt) {
                    auto newState = state; // need to keep a copy of each state instance
                    newState.growStash(dt);
                    newState.buildRobot(material(i));
                    best = std::max(best, simulate(time - dt, newState));
                }
            }
        }
        return best;
    }
};

//-----------------------------------------------------------------------------

int64_t solvePartOne(const std::vector<Blueprint>& blueprints) {
    int64_t result = 0;
    for (const auto& bp : blueprints) {
        State state{ bp };
        result += bp.id * State::simulate(24, state);
    }
    return result;
}

int64_t solvePartTwo(const std::vector<Blueprint>& blueprints) {
    int64_t result = 1;
    for (int64_t i = 0; i < 3; ++i) {
        result *= State::simulate(32, State{ blueprints[i] });
    }
    return result;
}

//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    common::assert(argc >= 2, "No inputfile provided");

    std::vector<Blueprint> blueprints;
    bool res = common::iterateFile(argv[1], [&blueprints](const std::string& line){
        blueprints.push_back(makeBlueprint(line));
    });
    common::assert(res, "Failed to iterate file");

    std::cout << "[Part I] Result: " << solvePartOne(blueprints) << "\n";
    std::cout << "[Part 2] Result: " << solvePartTwo(blueprints) << "\n";

    return 0;
}
