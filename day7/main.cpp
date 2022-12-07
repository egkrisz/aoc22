#include <algorithm>
#include <cstdint>
#include <functional>
#include <numeric>
#include <vector>

#include "../common/common.h"

struct File {
    std::string name;
    size_t size;

    File(const std::string& name, unsigned long size) : name(name), size(size) {}
};

struct Directory {
    Directory               *root = nullptr;
    std::string             name;
    std::vector<File>       files;
    std::vector<Directory*> dirs;

    Directory* addDirectory(const std::string& name) {
        auto dir = new Directory{ this, name };
        auto it = std::find_if(dirs.cbegin(), dirs.cend(), [dir](const Directory *directory) { return directory->name == dir->name; });
        if (it != dirs.cend()) {
            return *it;
        }
        dirs.push_back(dir);
        return dir;
    }

    size_t getTotalSize() const {
        return std::accumulate(files.cbegin(), files.cend(), 0U,
                               [](std::size_t result, auto file) { return result + file.size; })
             + std::accumulate(dirs.cbegin(), dirs.cend(), 0U,
                               [](std::size_t result, auto dir) { return result + dir->getTotalSize(); });
    }

    void print(const std::string& prefix = "") const {
        common::print(prefix, name, " (dir)");
        for (auto& file : files) {
            common::print(prefix, "  ", file.name, " (file, size=", file.size, ")");
        }
        for (auto dir : dirs) {
            dir->print(prefix + "  ");
        }
    }

    void iterate(std::function<void(Directory*)> func) {
        for (auto dir : dirs) {
            dir->iterate(func);
        }
        func(this);
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

    Directory *root = new Directory{ nullptr, "" };
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
                currentDir->files.emplace_back(toks[1], (size_t)std::stoul(toks[0]));
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
