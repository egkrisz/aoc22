#include <cstdint>
#include <compare>
#include <string_view>
#include <variant>
#include <vector>

#include "../common/common.h"

// what the bloody hell...
struct Packet : std::variant<std::vector<Packet>, int32_t> {
    using list = std::vector<Packet>;
    using val = int32_t;

	using variant::variant;
    explicit Packet(val v) : variant(v) { }
    explicit Packet(const list& list) : variant(list) {}
    explicit Packet(val&& v) : variant(std::move(v)) { }
    explicit Packet(list&& list) : variant(std::move(list)) {}

    friend std::strong_ordering operator<=>(const Packet& lhs, const Packet& rhs);

    friend std::ostream& operator<<(std::ostream& s, Packet& packet) {
        if (packet.index() == 0) {
            auto& l = std::get<Packet::list>(packet);
            s << '[';
            for (auto& elem : l) {
                s << elem << ',';
            }
            s << ']';
        } else {
            s << std::get<Packet::val>(packet);
        }
        return s;
    }
};

using inputFormat_t = std::vector<std::vector<std::string>>;
using packet_t = Packet;
using packetPair_t = std::pair<packet_t, packet_t>;

std::strong_ordering comp(const packet_t& first, const packet_t& second) {
    if (first.index() == 1 && second.index() == 1) {
        return std::get<packet_t::val>(first) <=> std::get<packet_t::val>(second);
    } else if (first.index() == 0 && second.index() == 0) {
        auto lhs = std::get<packet_t::list>(first);
        auto rhs = std::get<packet_t::list>(second);
        size_t listSize = std::min(lhs.size(), rhs.size());
        for (size_t i = 0; i < listSize; ++i) {
            auto res = comp(lhs[i], rhs[i]);
            if (res != std::strong_ordering::equivalent) {
                return res;
            }
        }
        return (lhs.size() <=> rhs.size());
    }

    packet_t::list lhs, rhs;
    if (first.index() == 1) {
        lhs.emplace_back(std::get<packet_t::val>(first));
        rhs = std::get<packet_t::list>(second);
    } else {
        lhs = std::get<packet_t::list>(first);
        rhs.emplace_back(std::get<packet_t::val>(second));
    }
    return comp(lhs, rhs);
}

std::strong_ordering operator<=>(const Packet& lhs, const Packet& rhs) {
    return comp(lhs, rhs);
}

packet_t parse(std::string_view& s) {
    if (s[0] == '[' && s[1] == ']') {
        s.remove_prefix(2);
        return packet_t{ packet_t::list{} };
    }
    if (s[0] == '[') {
        packet_t::list list;
        while (s[0] != ']') {
            s.remove_prefix(1);
            list.emplace_back(parse(s));
        }
        s.remove_prefix(1);
        return list;
    }
    const auto* it = std::find_if(s.cbegin(), s.cend(), [](const auto& c) { return c == ',' || c == ']'; });
    std::string v{ s.begin(), it };
    auto val = std::stoi(v);
    s.remove_prefix(v.size());

    return packet_t{ val };
}

packet_t parse(const std::string& s) {
    std::string_view v{ s.data(), s.size() };
    return parse(v);
}

size_t solvePartOne(const inputFormat_t& inputList) {
    size_t result = 0, index = 0;
    for (const auto& pair : inputList) {
        ++index;
        packetPair_t packetPair;
        packetPair.first = parse(pair[0]);
        packetPair.second = parse(pair[1]);

        if (packetPair.first <=> packetPair.second == std::strong_ordering::less) {
            result += index;
        }
    }
    return result;
}

size_t solvePartTwo(inputFormat_t inputList) {
    inputList.push_back({ "[[2]]", "[[6]]" });
    std::vector<packet_t> packetList{};
    for (const auto& pair : inputList) {
        packetList.emplace_back(parse(pair[0]));
        packetList.emplace_back(parse(pair[1]));
    }
    std::sort(packetList.begin(), packetList.end());

    size_t result = 1, index = 0;
    for (auto p : packetList) {
        index++;
        auto lst = std::get<packet_t::list>(p);
        if (lst.size() == 1 && lst[0].index() == 0) {
            auto inner = std::get<packet_t::list>(lst[0]);
            if (inner.size() == 1 && inner[0].index() == 1) {
                auto val = std::get<packet_t::val>(inner[0]);
                if (val == 2 || val == 6) {
                    result *= index;
                }
            }
        }
    }
    return result;
}

int main(int argc, char *argv[]) {
    common::assert(argc >= 2, "No inputfile provided");

    inputFormat_t inputList{ { } };
    bool res = common::iterateFile(argv[1], [&inputList](const std::string& line){
            if (line.empty()) {
                inputList.emplace_back();
                return;
            }
            inputList.back().push_back(line);
        });
    common::assert(res, "Failed to iterate file");

    std::cout << "[Part I] Result: " << solvePartOne(inputList) << "\n";
    std::cout << "[Part 2] Result: " << solvePartTwo(inputList) << "\n";

    return 0;
}
