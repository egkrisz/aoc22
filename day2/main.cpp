#include <algorithm>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <map>
#include <vector>

#include "../common/common.h"

enum Shape : uint64_t {
    Invalid = 0,
    Rock = 1,
    Paper = 2,
    Scissors = 3
};

enum Result : uint64_t {
    Lose = 0,
    Draw = 3,
    Win = 6
};

static std::map<Result, std::vector<std::pair<Shape, Shape>>> stateMachine = {
    { Lose, { { Rock, Scissors }, { Scissors, Paper },    { Paper, Rock     } } },
    { Draw, { { Rock, Rock },     { Scissors, Scissors }, { Paper, Paper    } } },
    { Win,  { { Rock, Paper },    { Scissors, Rock },     { Paper, Scissors } } }
};

Shape translateOppShape(char opp) {
    switch (opp) {
        case 'A': return Rock;
        case 'B': return Paper;
        case 'C': return Scissors;
    }
    return Invalid;
}

Shape translateOwnShape(char own) {
    switch (own) {
        case 'X': return Rock;
        case 'Y': return Paper;
        case 'Z': return Scissors;
    }
    return Invalid;
}

Result translateOwnResult(char own) {
    switch (own) {
        case 'X': return Lose;
        case 'Y': return Draw;
        case 'Z': return Win;
    }
    return Lose;
}

bool isResult(Result result, Shape opp, Shape own) {
    auto sm = stateMachine.at(result);
    return (std::find(sm.begin(), sm.end(), std::pair<Shape, Shape>{ opp, own }) != sm.end());
}

Shape getOwnShapeFromResult(Shape opp, Result result) {
    auto sm = stateMachine.at(result);
    auto it = std::find_if(sm.begin(), sm.end(), [&opp](const std::pair<Shape, Shape>& elem){ return elem.first == opp; });
    if (it != sm.end()) {
        return it->second;
    }
    return Invalid;
}

uint64_t getScoreI(char opp, char you) {
    uint64_t score = 0;

    Shape oppShape = translateOppShape(opp);
    Shape ownShape = translateOwnShape(you);

    score += ownShape;

    if (oppShape == ownShape) score += Draw;
    else if (isResult(Win, oppShape, ownShape)) score += Win;

    return score;
}

uint64_t getScoreII(char opp, char you) {
    Result result = translateOwnResult(you);
    Shape oppShape = translateOppShape(opp);
    Shape ownShape = getOwnShapeFromResult(oppShape, result);

    return ownShape + result;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "No inputfile provided\n";
        abort();
    }

    uint64_t scoreI = 0;
    uint64_t scoreII = 0;
    bool res = common::iterateFile(argv[1], [&scoreI, &scoreII](const std::string& line){
            std::vector<std::string> round;
            common::tokenize(line, round, ' ');
            std::cout << "Opponent: " << round[0] << " You: " << round[1] << '\n';
            scoreI += getScoreI(round[0][0], round[1][0]);
            scoreII += getScoreII(round[0][0], round[1][0]);
        });

    if (!res) {
        abort();
    }

    //================================ PART ONE ====================================
    std::cout << "[Part I] Total score: " << scoreI << '\n';

    //================================ PART TWO ====================================
    std::cout << "[Part II] Total score: " << scoreII << '\n';

    return 0;
}
