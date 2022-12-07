#include <algorithm>
#include <cstdint>
#include <functional>
#include <numeric>
#include <vector>

#include "../common/common.h"

struct File {
    std::string name;
    size_t size;
};

struct Directory {
    Directory               *root = nullptr;
    std::string             name;
    std::vector<File>       files;
    std::vector<Directory*> dirs;

    Directory* addDirectory(const std::string& name) {
        auto it = std::find_if(dirs.cbegin(), dirs.cend(), [&name](const Directory *dir) { return dir->name == name; });
        if (it != dirs.cend()) {
            return *it;
        }
        dirs.push_back(new Directory{ this, name });
        return dirs.back();
    }

    size_t getTotalSize() const {
        return std::accumulate(files.cbegin(), files.cend(), 0U,
                               [](std::size_t result, auto file) { return result + file.size; })
             + std::accumulate(dirs.cbegin(), dirs.cend(), 0U,
                               [](std::size_t result, auto dir) { return result + dir->getTotalSize(); });
    }

    template<typename Fn>
    void iterate(Fn func) {
        for (auto dir : dirs) {
            dir->iterate(func);
        }
        func(this);
    }

    void print(const std::string& prefix = "") {
        common::print(prefix, name, " (dir)");
        for (auto& file : files) {
            common::print(prefix, "  ", file.name, " (file, size=", file.size, ")");
        }
        for (auto dir : dirs) {
            dir->print(prefix + "  ");
        }
    }
};

size_t solvePartOne(Directory *root) {
    size_t result = 0;
    const size_t limit = 100000;
    root->iterate([&result, limit](Directory* dir) {
            auto sz = dir->getTotalSize();
            if (sz <= limit) result += sz;
        });
    return result;
}

size_t solvePartTwo(Directory *root) {
    const size_t unusedSpace = 70000000 - root->getTotalSize();
    const size_t minimumRemove = 30000000 - unusedSpace;

    size_t result = UINT64_MAX;
    root->iterate([&result, minimumRemove](Directory* dir) {
            auto sz = dir->getTotalSize();
            if (sz >= minimumRemove && sz < result) result = sz;
        });
    return result;
}

int main(int argc, char *argv[]) {
    common::assert(argc >= 2, "No inputfile provided");

    Directory *root = new Directory{ nullptr, "" }; // Dummy root
    Directory *currentDir = root;
    bool res = common::iterateFile(argv[1], [&currentDir](const std::string& line){
            auto toks = common::tokenize(line);
            if (toks[0] == "$") {
                if (toks[1] == "cd") {
                    if (toks[2] == "..") {
                        currentDir = currentDir->root;
                    } else {
                        currentDir = currentDir->addDirectory(toks[2]);
                    }
                }
            } else if (toks[0] == "dir") {
                currentDir->addDirectory(toks[1]);
            } else {
                currentDir->files.push_back({ toks[1], (size_t)std::stoul(toks[0]) });
            }
        });
    common::assert(res, "Failed to iterate file");

    root = root->dirs[0];
    root->print();

    std::cout << "[Part I] Result: " << solvePartOne(root) << "\n";
    std::cout << "[Part 2] Result: " << solvePartTwo(root) << "\n";

    //cleanup
    root->root->iterate([](Directory* dir){ delete dir; });

    return 0;
}
