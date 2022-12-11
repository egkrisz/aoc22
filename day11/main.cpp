#include <algorithm>
#include <cctype>
#include <cstdint>
#include <deque>
#include <sstream>
#include <numeric>
#include <vector>

#include "../common/common.h"

using item_t = int64_t;
using closure_t = std::function<item_t(item_t)>;

closure_t makeAdder(item_t val, bool self) {
    return [val, self] (item_t old) -> item_t {
        if (self) {
            return old * 2;
        }
        return val + old;
    };
}

closure_t makeMultiplier(item_t val, bool self) {
    return [val, self] (item_t old) -> item_t {
        if (self) {
            return old * old;
        }
        return val * old;
    };
}

closure_t makeTester(item_t divisor, item_t targetIfTrue, item_t targetIfFalse) {
    return [divisor, targetIfTrue, targetIfFalse] (item_t worryLevel) {
        if (worryLevel % divisor == 0) {
            return targetIfTrue;
        }
        return targetIfFalse;
    };
}

struct Monkey {
    size_t             id{ 0 };
    size_t             inspectionCount{ 0 };
    item_t             divisor{ 1 };
    closure_t          operation{ nullptr };
    closure_t          test{ nullptr };
    std::deque<item_t> items;

    void init(size_t mId, const std::vector<std::string>& params) {
        common::assert(params.size() == 6, "invalid parameter set passed to 'Monkey::init()'");
        id = mId;
        setItems(params[1]);
        setOperation(params[2]);
        setTest(params[3], params[4], params[5]);
    }
    void setItems(const std::string& itemStr) {
        auto itemsTok = common::tokenize(itemStr);
        std::transform((itemsTok.cbegin() + 2), itemsTok.cend(), std::back_inserter(items),
                       [](const std::string& item) { return std::stoi(common::strip(item)); });
    }
    void setOperation(const std::string& opStr) {
        char op;
        item_t var = 0;
        std::sscanf(opStr.c_str(), "Operation: new = old %c %ld", &op, &var);

        bool self = (var == 0);
        if (op == '+') {
            operation = makeAdder(var, self);
        } else if (op == '*') {
            operation = makeMultiplier(var, self);
        } else {
            common::assert(0, "Invalid operator");
        }
    };
    void setTest(const std::string& condStr, const std::string& trueStr, const std::string& falseStr) {
        item_t targetIfTrue = 0, targetIfFalse = 0;
        std::sscanf(condStr.c_str(), "Test: divisible by %ld", &divisor);
        std::sscanf(trueStr.c_str(), "If true: throw to monkey %ld", &targetIfTrue);
        std::sscanf(falseStr.c_str(), "If false: throw to monkey %ld", &targetIfFalse);

        test = makeTester(divisor, targetIfTrue, targetIfFalse);
    }
    void inspect(std::vector<Monkey>& monkeys, item_t divisor = 3, item_t mod = 0) {
        while (!items.empty()) {
            inspectionCount++;
            auto worryLevel = operation(items.front()) / divisor;
            if (mod > 0) {         // Hack to prevent int overflow:
                worryLevel %= mod; // Use the least common multiple of all divisors to rationalize big ints
            }
            items.pop_front();
            monkeys[test(worryLevel)].items.push_back(worryLevel);
        }
    }
    void printItems() const {
        std::stringstream ss;
        for (auto item : items) {
            ss << item << ", ";
        }
        common::print("Monkey ", id, ": ", ss.str());
    }
};

size_t solvePartOne(std::vector<Monkey> monkeys, size_t iterations = 20, item_t divisor = 3, item_t mod = 0) {
    for (size_t i = 0; i < iterations; ++i) {
        for (auto& monkey : monkeys) {
            monkey.inspect(monkeys, divisor, mod);
        }
    }
    std::sort(monkeys.begin(), monkeys.end(), [](const Monkey& lhs, const Monkey& rhs) { return lhs.inspectionCount > rhs.inspectionCount; });
    return monkeys[0].inspectionCount * monkeys[1].inspectionCount;
}

size_t solvePartTwo(std::vector<Monkey> monkeys) {
    item_t mod = 1;
    std::for_each(monkeys.cbegin(), monkeys.cend(), [&mod](const Monkey& monkey){ mod = std::lcm(mod, monkey.divisor); });
    return solvePartOne(monkeys, 10000, 1, mod);
}

int main(int argc, char *argv[]) {
    common::assert(argc >= 2, "No inputfile provided");

    std::vector<std::vector<std::string>> monkeyParams = { { } };
    bool res = common::iterateFile(argv[1], [&monkeyParams](const std::string& line){
             if (line.empty()) {
                 monkeyParams.push_back({ });
                 return;
             }
             monkeyParams.back().push_back(common::ltrim(line));
        });
    common::assert(res, "Failed to iterate file");

    std::vector<Monkey> monkeys(monkeyParams.size());
    for (size_t i = 0; i < monkeys.size(); ++i) {
        monkeys[i].init(i, monkeyParams[i]);
    }

    std::cout << "[Part I] Result: " << solvePartOne(monkeys) << "\n";
    std::cout << "[Part 2] Result: " << solvePartTwo(monkeys) << "\n";

    return 0;
}
