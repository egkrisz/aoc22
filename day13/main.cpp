#include <algorithm>
#include <cstdint>
#include <variant>
#include <vector>

#include "../common/common.h"

// what the bloody hell...
struct Packet : std::variant<std::vector<Packet>, int32_t> {
    using list = std::vector<Packet>;
    using val = int32_t;

    Packet* parent = nullptr;

	using variant::variant;
    explicit Packet(val v) : variant(v) { }
    explicit Packet(const list& list) : variant(list) {}
    explicit Packet(val&& v) : variant(std::move(v)) { }
    explicit Packet(list&& list) : variant(std::move(list)) {}

    auto getList() -> list& { return std::get<list>(*this); }
    auto getVal()  -> val&  { return std::get<val>(*this); }

    /* friend bool operator<(const Packet& lhs, const Packet& rhs); */

    friend std::ostream& operator<<(std::ostream& s, Packet& packet) {
        if (packet.index() == 0) {
            auto& l = packet.getList();
            s << '[';
            for (auto& elem : l) {
                s << elem << ',';
            }
            s << ']';
        } else {
            s << packet.getVal();
        }
        return s;
    }
};

using inputFormat_t = std::vector<std::vector<std::string>>;
using packet_t = Packet;
using packetPair_t = std::pair<packet_t, packet_t>;

/*
struct PacketVisitor {
    bool operator()(packet_t::val lhs, packet_t::val rhs) const {
        return lhs < rhs;
    }
    bool operator()(const packet_t::list& lhs, const packet_t::list& rhs) const {
        return std::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }
    bool operator()(packet_t::val lhs, const std::vector<packet_t>& rhs) const {
        return (*this)(packet_t::list{ lhs }, rhs);
    }
    bool operator()(const std::vector<packet_t>& lhs, packet_t::val rhs) const {
        return (*this)(lhs, packet_t::list{ rhs });
    }
};


// from https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2020/p2162r0.html#ref-P1371R2
namespace std {
    template <> struct variant_size<Packet> : variant_size<Packet::variant> {};
    
    template <std::size_t I> struct variant_alternative<I, Packet> : variant_alternative<I, Packet::variant> {};
}

bool operator<(const packet_t& lhs, const packet_t& rhs) {
    return std::visit(PacketVisitor{}, lhs, rhs);
}
*/

void parseChar(std::string::const_iterator it, std::string::const_iterator end, packet_t& packet) {
    if (it == end) {
        return;
    }
    if (*it == ',') {
        ++it;
    }

    auto& list = std::get<packet_t::list>(packet);
    if (*it == '[') {
        list.emplace_back(std::vector<packet_t>{});
        list.back().parent = &packet;
        parseChar(++it, end, list.back());
    } else if (*it == ']') {
        parseChar(++it, end, *packet.parent);
    } else if (std::isdigit(*it)) {
        int32_t val = *it - '0';
        list.emplace_back(val);
        parseChar(++it, end, packet);
    }
}

bool comp(const packet_t& first, const packet_t& second) {
    if (first.index() == 1 && second.index() == 1) {
        return std::get<packet_t::val>(first) < std::get<packet_t::val>(second);
    } else if (first.index() == 0 && second.index() == 0) {
        auto lhs = std::get<packet_t::list>(first);
        auto rhs = std::get<packet_t::list>(second);
        return std::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
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

enum class Ordering { Less, Equal, Greater };
Ordering compare(const packet_t& first, const packet_t& second) {
    if (first.index() == 1 && second.index() == 1) {
        common::print("COMPARING: ", std::get<1>(first), ", ", std::get<1>(second));

        if (std::get<packet_t::val>(first) < std::get<packet_t::val>(second)) {
            return Ordering::Less;
        } else if (std::get<packet_t::val>(first) > std::get<packet_t::val>(second)) {
            return Ordering::Greater;
        } else {
            return Ordering::Equal;
        }
    }

    std::vector<packet_t> firstList, secondList;
    if (first.index() == 0) {
        firstList = std::get<packet_t::list>(first);
    } else {
        firstList.emplace_back(std::get<packet_t::val>(first));
    }
    if (second.index() == 0) {
        secondList = std::get<packet_t::list>(second);
    } else {
        secondList.emplace_back(std::get<packet_t::val>(second));
    }

    size_t listSize = std::min(firstList.size(), secondList.size());
    for (size_t i = 0; i < listSize; ++i) {
        auto res = compare(firstList[i], secondList[i]);
        if (res != Ordering::Equal) {
            return res;
        }
    }

    if (firstList.size() < secondList.size()) {
        return Ordering::Less;
    } else if (firstList.size() > secondList.size()) {
        return Ordering::Greater;
    }
    return Ordering::Equal;
}

size_t solvePartOne(const inputFormat_t& inputList) {
    size_t result = 0, index = 0;
    for (const auto& pair : inputList) {
        ++index;
        packetPair_t packetPair;
        parseChar(pair[0].cbegin() + 1, pair[0].cend() - 1, packetPair.first);
        parseChar(pair[1].cbegin() + 1, pair[1].cend() - 1, packetPair.second);

        common::print(packetPair.first);
        common::print(packetPair.second);

        /* if (compare(packetPair.first, packetPair.second) == Ordering::Less) { */
        if (comp(packetPair.first, packetPair.second)) {
            common::print(index, ": IS IN RIGHT ORDER");
            result += index;
        } else {
            common::print(index, ": NOT IN RIGHT ORDER");
        }
        /*if (packetPair.first < packetPair.second) {
            common::print(index, ": IS IN RIGHT ORDER");
            result += index;
        } else {
            common::print(index, ": NOT IN RIGHT ORDER");
        }*/
        common::print("");
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
    // std::cout << "[Part 2] Result: " << resultII << "\n";

    return 0;
}
