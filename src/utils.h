#pragma once

#include "options.h"

#include <ostream>
#include <vector>

#define EMPTY 0
#define BLACK 1
#define WHITE 2

#define E 0
#define B 1
#define W 2

//Return number of opposing player (1 --> 2), (2 --> 1)
inline int opponentNumber(int n) { //Inline means this needs to be in the header
    if (n == 1 || n == 2) {
        return (n % 2) + 1;
    }
    return n;
}

// (1 --> 1), (2 --> -1)
int playerSign(int p);

// ('B' --> 'W'), ('W' --> 'B')
char opponentChar(char c);

// (0 --> '.'), (1 --> 'B'), (2 --> 'W')
char playerNumberToChar(int n);

// ('.' --> 0), ('B' --> 1), ('W' --> 2)
int charToPlayerNumber(char c);

//ostream operator for std::pair
template <class T1, class T2>
std::ostream &operator<<(std::ostream &os, const std::pair<T1, T2> &p) {
    os << "(" << p.first << " " << p.second << ")";
    return os;
}

template <class T>
int sign(T &val) {
    return T(0) > val ? -1 : 1;
}

template <class T>
int sumBits(T val) {
    int sum = 0;
    
    while (val != 0) {
        sum += val & T(1);
        val >>= 1;
    }

    return sum;
}

template <class T1, class T2, class T3>
struct triple {
    T1 first;
    T2 second;
    T3 third;

    triple(T1 first, T2 second, T3 third) {
        this->first = first;
        this->second = second;
        this->third = third;
    }

};

template <class T1, class T2, class T3>
bool operator<(const triple<T1, T2, T3> &t1, const triple<T1, T2, T3> &t2) {
    if (t1.first < t2.first) {
        return true;
    }
    if (t1.first > t2.first) {
        return false;
    }

    if (t1.second < t2.second) {
        return true;
    }
    if (t1.second > t2.second) {
        return false;
    }

    if (t1.third < t2.third) {
        return true;
    }
    if (t1.third > t2.third) {
        return false;
    }

    return false;
}

size_t gameLength(size_t bufferSize, char *game);

void negateGame(size_t length, char *game);


char *generateGame(int length, int game);

char *generateGameFromShape(uint64_t shape, int game);

uint64_t shapeToNumber(const std::vector<int> &svec);
uint64_t shapeDataToID(const std::vector<std::pair<int, char *>> &shapeData);

std::vector<int> numberToShape(uint64_t snum);

template <class T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vec) {
    os << "[";
    for (int i = 0; i < vec.size(); i++) {
        os << vec[i];
        if (i + 1 < vec.size()) {
            os << ", ";
        }
    }
    os << "]";

    return os;
}

uint64_t shapeToNumber(const std::vector<int> &shape);
std::vector<int> numberToShape(uint64_t number);


////////////////////////////// Bitvector stuff

struct __BitvectorBracket;
struct __BitvectorBracketConst;

struct Bitvector {
    uint64_t data[BIT_VECTOR_SIZE];
    static const int size = BIT_VECTOR_SIZE * 64;

    Bitvector();
    void operator=(const Bitvector &v);

    __BitvectorBracket operator[](int i);
    __BitvectorBracketConst operator[](int i) const;
};

struct __BitvectorBracket {
    uint64_t *data;
    int i;

    __BitvectorBracket(uint64_t *data, int i);
    void operator=(bool val);
    operator bool();
};

struct __BitvectorBracketConst {
    const uint64_t *data;
    int i;

    __BitvectorBracketConst(const uint64_t *data, int i);
    operator bool() const;
};


std::ostream &operator<<(std::ostream &os, const Bitvector &v);
