#include "utils.h"
#include <iostream>
#include <vector>
#include "database2.h"

using namespace std;

int playerSign(int p) {
    return p == 1 ? 1 : -1;
}

char opponentChar(char c) {
    switch (c) {
        case 'B':
            return 'W';
            break;

        case 'W':
            return 'B';
            break;
    }

    return '.';
}

char playerNumberToChar(int n) {
    switch (n) {
        case BLACK:
            return 'B';
            break;

        case WHITE:
            return 'W';
            break;
    }

    return '.';
}

int charToPlayerNumber(char c) {
    switch (c) {
        case 'B':
            return BLACK;
            break;

        case 'W':
            return WHITE;
            break;
    }

    return EMPTY;
}


size_t gameLength(size_t bufferSize, char *game) {
    size_t idx = 0;

    for (size_t i = 0; i < bufferSize; i++) {
        if (game[i] != 0) {
            idx = i;
        }
    }

    return idx + 1;
}


void negateGame(size_t length, char *game) {
    for (size_t i = 0; i < length; i++) {
        game[i] = opponentNumber(game[i]);
    }
}

char *generateGame(int length, int game) {
    char *board = new char[length + 1];
    board[length] = 0;

    for (int i = 0; i < length; i++) {
        if ((game >> i) & 1) {
            board[i] = WHITE;
        } else {
            board[i] = BLACK;
        }
    }

    return board;
}


////////////////////////////////////////New DB stuff

//TODO implement this
char *generateGameFromShape(uint64_t shape, int game) {
    char *board = new char[4];
    return board;
}

////////////////////////////////////////////////////////////
//THESE FUNCTIONS MUST BE CHANGED TOGETHER


uint64_t shapeVectorToNumber(const vector<int> &svec) {
    uint64_t snum = 0;
    uint64_t shift = _shiftAmount();

    for (auto it = svec.rbegin(); it != svec.rend(); it++) {
        snum <<= shift;
        int chunkSize = *it - 1;
        snum += chunkSize;
    }

    return snum;
}

//Used by database get function
uint64_t shapeDataToID(const vector<pair<int, char *>> &shapeData) {
    uint64_t snum = 0;
    uint64_t shift = _shiftAmount();

    for (auto it = shapeData.rbegin(); it != shapeData.rend(); it++) {
        snum <<= shift;
        int chunkSize = it->first - 1;
        snum += chunkSize;
    }
    

    return snum;
}
////////////////////////////////////////////////////////////

vector<int> numberToShapeVector(uint64_t snum) {
    vector<int> svec;
    uint64_t shift = _shiftAmount();

    uint64_t mask = (1 << shift) - 1;

    while (snum != 0) {
        int chunkSize = (snum & mask) + 1;
        snum >>= shift;
        svec.push_back(chunkSize);
    }

    return svec;
}


