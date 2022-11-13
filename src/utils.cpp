#include "utils.h"
#include <iostream>
#include <vector>
#include <cstring>
#include "database3.h"

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

    assert(c == '.');
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

    assert(n == 0);
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

    assert(c == '.');
    return EMPTY;
}


void negateBoard(uint8_t *board, size_t length) {
    for (size_t i = 0; i < length; i++) {
        board[i] = opponentNumber(board[i]);
    }
}


void printBoard(uint8_t *board, size_t len, bool newline) {
    cout << "{ ";
    for (size_t i = 0; i < len; i++) {
        cout << playerNumberToChar(board[i]) << " ";
    }
    
    cout << "}";
    if (newline) {
        cout << endl;
    }
}
