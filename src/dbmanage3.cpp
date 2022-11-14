#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <map>

#include "solver.h"
#include "database3.h"
#include "utils.h"
#include "state.h"

using namespace std;

Database *db;
Solver *solver;

void computeDominance(uint8_t *g1, size_t g1Size, uint64_t *dominance) {
    uint64_t domBlack = 0;
    uint64_t domWhite = 0;

    uint8_t g2[g1Size];
    size_t g2Size = g1Size;
    memcpy(g2, g1, g1Size);

    uint8_t undo1[UNDO_BUFFER_SIZE];
    uint8_t undo2[UNDO_BUFFER_SIZE];

    //Compute dominance for black
    {
        size_t moveCount;
        int *moves = getMoves(g1, g1Size, 1, &moveCount);

        for (size_t i = 0; i < moveCount; i++) {
            play(g1, undo1, moves[2 * i], moves[2 * i + 1]);
            for (size_t j = i + 1; j < moveCount; j++) {
                play(g2, undo2, moves[2 * j], moves[2 * j + 1]);
                negateBoard(g2, g2Size);

                size_t g3Size;
                uint8_t *g3 = addGames(g1, g1Size, g2, g2Size, &g3Size); 
                int bFirst = solver->solveID(g3, g3Size, 1);
                int wFirst = solver->solveID(g3, g3Size, 2);
                delete[] g3;


                if (bFirst == wFirst) {
                    if (bFirst == 1) { // I - J is positive for black --> I > J
                        domBlack |= (((uint64_t) 1) << j);
                    } else { // I - J is negative for black --> I < J
                        domBlack |= (((uint64_t) 1) << i);
                    }
                }


                negateBoard(g2, g2Size);
                undo(g2, undo2);
            }
            undo(g1, undo1);
        }

        if (moveCount) {
            delete[] moves;
        }

    }

    //Compute dominance for white
    {
        size_t moveCount;
        int *moves = getMoves(g1, g1Size, 2, &moveCount);

        for (size_t i = 0; i < moveCount; i++) {
            play(g1, undo1, moves[2 * i], moves[2 * i + 1]);
            for (size_t j = i + 1; j < moveCount; j++) {
                play(g2, undo2, moves[2 * j], moves[2 * j + 1]);
                negateBoard(g2, g2Size);

                size_t g3Size;
                uint8_t *g3 = addGames(g1, g1Size, g2, g2Size, &g3Size); 
                int bFirst = solver->solveID(g3, g3Size, 1);
                int wFirst = solver->solveID(g3, g3Size, 2);
                delete[] g3;

                if (bFirst == wFirst) {
                    if (bFirst == 2) { // I - J is positive for white --> I > J
                        domWhite |= (((uint64_t) 1) << j);
                    } else { // I - J is negative for white --> I < J
                        domWhite |= (((uint64_t) 1) << i);
                    }
                }


                negateBoard(g2, g2Size);
                undo(g2, undo2);
            }
            undo(g1, undo1);
        }

        if (moveCount) {
            delete[] moves;
        }

    }

    dominance[0] = domBlack;
    dominance[1] = domWhite;

}


void computeBounds(uint8_t *board, size_t boardLen, int8_t *bounds) {
    uint8_t compare[32];

    map<int, int> ltFlags; //-1 false, 0 unknown, 1 true
    map<int, int> gtFlags;

    bool found = false;

    for (int i = 0; abs(i) < 31; i = i >= 0 ? -(i + 1) : -i) {

        memset(compare, 0, 32);
        int player = i < 0 ? 1 : 2; //we're adding the negative of compare -- flip 1 and 2


        compare[0] = opponentNumber(player);

        for (int j = 0; j < abs(i); j++) {
            compare[j + 1] = player;
        }

        //char *g = addGames(strlen(board), board, strlen(compare), compare);
        size_t gLen;
        uint8_t *g = addGames(board, boardLen, compare, strlen((char *) compare), &gLen);

        //int boardSize = strlen(board) + 1 + strlen(compare);
        //int result1 = gameResult(db, g, boardSize, 1);
        //int result2 = gameResult(db, g, boardSize, 2);

        int result1 = solver->solveID(g, gLen, 1);
        int result2 = solver->solveID(g, gLen, 2);

        delete[] g;

        int outcomeClass;
        if (result1 == result2) {
            outcomeClass = result1;
        } else {
            if (result1 == 1) {
                outcomeClass = OC_N;
            } else {
                outcomeClass = OC_P;
            }
        }


        // C >= G --> 0 >= G - C --> G - C <= 0
        gtFlags[i] = (outcomeClass == OC_W || outcomeClass == OC_P) ? 1 : -1;

        // C <= G --> 0 <= G - C --> G - C >= 0
        ltFlags[i] = (outcomeClass == OC_B || outcomeClass == OC_P) ? 1 : -1;
 

        //Now check if we can determine the bound from this
        int low, high;
        bool foundLow = false;
        bool foundHigh = false;

        for (int j = -31; j < 31; j++) {
            if (ltFlags[j] == 1 && ltFlags[j + 1] == -1) {
                low = j;
                foundLow = true;
                break;
            }
            
        }

        for (int j = 31; j > -31; j--) {
            if (gtFlags[j] == 1 && gtFlags[j - 1] == -1) {
                high = j;
                foundHigh = true;
                break;
            }
        }

        if (foundLow && foundHigh) {
            bounds[0] = low;
            bounds[1] = high;
            found = true;
            break;
        }

    }

    if (!found || bounds[0] > bounds[1]) {
        cout << "Bounds not found..." << endl;
        cout << "{" << (int) bounds[0] << " " << (int) bounds[1] << "}" << endl;


        cout << "LT" << endl;
        for (int i = -32; i < 32; i++) {
            cout << "(" << i << " " << ltFlags[i] << ") ";
        }
        cout << endl;

        cout << "GT" << endl;
        for (int i = -32; i < 32; i++) {
            cout << "(" << i << " " << gtFlags[i] << ") ";
        }
        cout << endl;

        while (1) {
        }
    }

}


int main() {
    //Initialize: DB, solver, shapes
    db = new Database();
    db->init();

    solver = new Solver(DB_MAX_BITS, db);

    vector<vector<int>> shapeList = makeShapes();

    sort(shapeList.begin(), shapeList.end(),
        [](const vector<int> &s1, const vector<int> &s2) {
            int bits1 = s1.size() - 1;
            int bits2 = s2.size() - 1;

            for (int chunk : s1) {
                bits1 += chunk;
            }

            for (int chunk : s2) {
                bits2 += chunk;
            }

            return bits1 < bits2;
        }
    );

    //Iterate over all shapes
    for (const vector<int> &shape : shapeList) {
        uint64_t shapeNumber = shapeToNumber(shape);

        //iterate over all games
        int gameBits = 0;
        for (int chunk : shape) {
            gameBits += chunk;
        }

        uint32_t minGame = 0;
        uint32_t gameCount = 1 << gameBits;

        for (uint32_t gameNumber = minGame; gameNumber < gameCount; gameNumber++) {
            //Get game and print it
            uint8_t *board;
            size_t boardLen;
            makeGame(shapeNumber, gameNumber, &board, &boardLen);
            cout << "Shape " << shape << " (" << shapeNumber << ") ";
            printBoard(board, boardLen, true);

            //Get entry
            uint64_t idx = db->getIdx(board, boardLen);
            //cout << "idx " << idx << endl;
            uint8_t *entry = db->getFromIdx(idx);
            assert(entry);
            assert(*db_get_outcome(entry) == 0);

            //Set trivial values (self link, shape, number)
            *db_get_link(entry) = idx;
            *db_get_shape(entry) = shapeNumber;
            *db_get_number(entry) = gameNumber;

            //Compute outcome class
            int result1 = solver->solveID(board, boardLen, BLACK);
            int result2 = solver->solveID(board, boardLen, WHITE);

            uint8_t outcomeClass = OC_UNKNOWN;

            if (result1 == result2) {
                outcomeClass = result1;
            } else {
                if (result1 == BLACK) {
                    outcomeClass = OC_N;
                } else {
                    outcomeClass = OC_P;
                }
            }


            *db_get_outcome(entry) = outcomeClass;
            //cout << (int) outcomeClass << " " << (int) *db_get_outcome(entry) << endl;


            //compute dominance
            if (boardLen <= DB_MAX_DOMINANCE_BITS) {
                uint64_t dominance[2];
                computeDominance(board, boardLen, dominance);

                db_get_dominance(entry)[0] = dominance[0];
                db_get_dominance(entry)[1] = dominance[1];
            }


            
            //compute bounds
            if (boardLen <= DB_MAX_BOUND_BITS) {
                int8_t bounds[2];
                computeBounds(board, boardLen, bounds);
                db_get_bounds(entry)[0] = bounds[0];
                db_get_bounds(entry)[1] = bounds[1];
                cout << (int) bounds[0] << " <> " << (int) bounds[1] << endl;
            }
            


            //compute metric
            //add to map




            delete[] board;
        }


    }

    //do second pass to find links


    db->save();

    delete solver;
    delete db;
}
