#include <iostream>
#include "utils.h"
#include "state.h"
#include "solver.h"

using namespace std;


int main(int argc, char **argv) {
    if (argc < 4) {
        cout << "Usage:\n" << argv[0] << " <board> <toPlay> <time> [debug]" << endl;
        return 0;
    }

    string board(argv[1]);

    int rootPlayer = charToPlayerNumber(*argv[2]);
    BasicSolver solver(rootPlayer, board.length());
    State *root = new State(board, rootPlayer);

/*
    for (size_t i = 0; i < root->moveCount; i++) {
        cout << root->moves[2 * i] << " " << root->moves[2 * i + 1] << endl;
    }

    return 0;
*/

    int result = solver.solveRoot(root, rootPlayer, opponentNumber(rootPlayer));
    if (best_from==-1){
    cout << playerNumberToChar(result)<< " None"<<" "<<node_count;
    }
    else{
        cout << playerNumberToChar(result) <<" "<<best_from<<"-"<< best_to<<" "<<node_count;
    }
    


    return 0;
}
