#include <algorithm>
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

Shape translateShape(char opp) {
    switch (opp) {
        case 'A': case 'X': return Rock;
        case 'B': case 'Y': return Paper;
        case 'C': case 'Z': return Scissors;
    }
    return Invalid;
}

Result translateResult(char own) {
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

int getScoreI(char opp, char you) {
    Shape oppShape = translateShape(opp);
    Shape ownShape = translateShape(you);

    int score = ownShape;
    if (oppShape == ownShape) score += Draw;
    else if (isResult(Win, oppShape, ownShape)) score += Win;

    return score;
}

int getScoreII(char opp, char you) {
    Result result = translateResult(you);
    Shape oppShape = translateShape(opp);
    Shape ownShape = getOwnShapeFromResult(oppShape, result);

    return ownShape + result;
}

int main(int argc, char *argv[]) {
    common::assert(argc >= 2, "No inputfile provided");

    int scoreI = 0;
    int scoreII = 0;
    bool res = common::iterateFile(argv[1], [&scoreI, &scoreII](const std::string& line){
            std::vector<std::string> round = common::tokenize(line, ' ');
            scoreI += getScoreI(round[0][0], round[1][0]);
            scoreII += getScoreII(round[0][0], round[1][0]);
        });
    common::assert(res, "Failed to iterate file");

    std::cout << "[Part I] Total score: " << scoreI << '\n';
    std::cout << "[Part II] Total score: " << scoreII << '\n';

    return 0;
}
