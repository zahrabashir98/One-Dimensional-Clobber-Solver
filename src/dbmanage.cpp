#include <iostream>
#include <cstring>
#include "utils.h"
#include "state.h"
#include "solver.h"
#include "database.h"
#include <chrono>
#include <set>
#include <string>
#include <iostream>
#include <algorithm>
using namespace std;

bool all_black(string s){
    set <char> s1;
   
    for ( int i=0 ; i < s.length() ; i++){
        s1.insert(s[i]);}
     
    if ( s1.size() == 1 && s[0]=='B'){
        // cout << "YES";
        return true;
    }
    else{
        // cout << "NO";
        return false;
    }
}
bool all_white(string s){
    set <char> s1;
   
    for ( int i=0 ; i < s.length() ; i++){
        s1.insert(s[i]);}
     
    if ( s1.size() == 1 && s[0]=='W'){
        // cout << "YES";
        return true;
    }
    else{
        // cout << "NO";
        return false;
    }
}

int get_pattern_value(char* board, int length){
    // all patterns
    string s = "";
    int up_count = 0;
    int down_count = 0;
    int star_count = 0;

    for (int i = 0; i < length; i++) {
        s += board[i];

    }

    if (s[0] =='W' && all_black(s.substr(1, length))){
        //length-1 * up + lenth * star
        up_count += length-2;
        star_count += length-1;
        star_count = star_count%2;
        return (star_count + 10*down_count + 100*up_count);
    }
    else if(s[0] =='B' && all_white(s.substr(1, length))){
        //length-1 * down + lenth * star
        down_count += length-2;
        star_count += length-1;
        star_count = star_count%2;

        return (star_count + 10*down_count + 100*up_count);

    }
    else if(length%3==0){
        int count =0;
        for (int i = 0; i < length/3 ; i++) {
            if (s.substr(0+i*3, 3+i*3) =="BBW"){
                count +=1;}
        
        }
        if (count == length/3){
            return ((length/3 +1)/2) *100;
        }
         else{
            return VAL_UNK;
        }

        count = 0;
        for (int i = 0; i < length/3 ; i++) {
            if (s.substr(0+i*3, 3+i*3) =="WWB"){
                count +=1;
            }
    
        }
        if (count == length/3){
            return ((length/3 +1)/2) *100;
        }
        else{
            return VAL_UNK;
        }

    }

    else{

        return VAL_UNK;
    }
}



int gameResult(Database &db, char *board, int boardSize, int player) {
    int result;

    auto start = std::chrono::steady_clock::now();
    BasicSolver *solver = new BasicSolver(player, boardSize, &db);
    solver->timeLimit = 1000000000.0;
    solver->startTime = start;


    char boardText[boardSize + 1];
    memcpy(boardText, board, boardSize);
    boardText[boardSize] = 0;
    for (int i = 0; i < boardSize; i++) {
        boardText[i] = playerNumberToChar(boardText[i]);
    }

    State *root = new State(boardText, player);

    result = solver->solveID(root, player, opponentNumber(player));

    delete solver;
    delete root;

    return result;
}

int set_basic_value_rules(int L, int R){
    if (L==-1 || R==-1) return 0; //{ | } = 0
    int R_downs = int((R/10)%10);
    if (L==0 & R==0) return 1;
    else if (L==0 & R==1) return 100;
    else if (L==1 & R==0) return 10;
    else if (L==0 and R_downs==0){ //Rule 0 and 1
        //rule 0
        if(R%10==1){ // if R has one *
            int ups = R/100;
            return (ups+1)*100; //
        }
        //rule 1
        if(R%10==0){// R has no star
            int ups = R/100;
            return ((ups+1)*100+1); //
        }
    }
    else return VAL_UNK;
}

int compare(int value1, int value2, bool max){
    // cout<<"\ncompare"<<value1<<"  "<< value2 <<"\n";
//implement comparision
    int ups_1 =  int(value1/100);
    int downs_1 = int((value1/10)%10);
    int stars_1 = int(value1%10);

    int ups_2 =  int(value2/100);
    int downs_2 = int((value2/10)%10);
    int stars_2 = int(value2%10);

// it is just for testing -> should be well implemeneted
 if (max){
    //  cout<<"max\n";
     //take max
     // if we have star in both go according to comparision rule 2
     if (stars_1>0 && stars_2>0){
         if(ups_1>ups_2 && downs_2<downs_1){
             // value1>value2
            return value1;}
        else return value2;
         }
    // if both do not have star follow rule 1
    else if (stars_1==0 && stars_2==0){
         if(ups_1>ups_2 && downs_2<downs_1){
             // value1>value2
            return value1;}
        else return value2;
         }
    
    else return -1; // not comparabale!!!
    }
 
 
 else{
     //take min
    //  cout<<"min\n";
     // if we have star in both go according to comparision rule 2
     if (stars_1>0 && stars_2>0){
         if(ups_1<=ups_2 && downs_2<=downs_1){
             // value1>value2
            return value1;}
        else return value2;
         }
    // if both do not have star follow rule 1
    else if (stars_1==0 && stars_2==0){
         if(ups_1>ups_2 && downs_2<downs_1){
             // value1>value2
            return value1;}
        else return value2;
    }
    else{ 
        // cout<<"not comparable";
        return VAL_UNK; // not comparabale!!!
    }
 
 
 
 }


}

int simplifySumValue(int value){
    int ups=  int(value/100);
    int downs= int((value/10)%10);
    int stars = int(value%10);

    stars = stars%2;
    if (ups >= downs){
        ups = ups - downs;
        downs = 0;
    }else{
        downs = downs - ups;
        ups= 0;
    }

    int final_value = 100*ups + 10*downs + stars;

    return final_value;

}


int gameValue(Database &db, char *board, int boardSize, int player){
    // returns val_unk if the value cannot be computed, returns -1 if there are no options (empty options)
    // cout<<"finding game value \n";
    // play left or right base on the given player
    std:: vector<int> optionValues ;
    
    BasicSolver *solver = new BasicSolver(player, boardSize, &db);

    char boardText[boardSize + 1];
    memcpy(boardText, board, boardSize);
    boardText[boardSize] = 0;

    for (int i = 0; i < boardSize; i++) {      
        boardText[i] = playerNumberToChar(boardText[i]);
        
    }
    

    State *state = new State(boardText, player);
    // cout<<"board created\n";

    size_t moveCount;
    int *moves = state->getMoves(player, opponentNumber(player), &moveCount);
    // -1 means there is no move (empty) not zero
    if(moveCount==0){
        return -1;
    }
    // cout<<"moves";

    // first play

    char undoBuffer[sizeof(int) + 2 * sizeof(char)];
    
    bool No_value=false;

    for (size_t i = 0; i < moveCount; i++) {
        int from = moves[2 * i];
        int to = moves[2 * i + 1];
        // cout<<"from to"<< from<< to<<"\n";

        state->play(from, to, undoBuffer);
        // int result = solve(state, n, p);
        //solve on level

        //first simplify
        // solver->simplify(state);

        // solve each move 1 leve;
        uint64_t gameVal = 0;
        std::vector<std::pair<int, int>> subgames = generateSubgames(state);
        int subgameCount = subgames.size();
        

        // cout<<"subgameCount  "<<subgameCount << "\n";
        if(subgameCount==0) {optionValues.push_back(0);
        // cout<<"subgame is zero \n";
        }
        else{
            //for each subgame (probably 2) get the value from db and add them
        for (auto it = subgames.begin(); it != subgames.end(); it++) {
            int length = it->second - it->first;
            

            unsigned char *entry = db.get(length, &state->board[it->first]);
            int SubgameVal = DB_GET_VALUE(entry);
            
            // cout<<"subgame values "<< SubgameVal<<"\n";
            // with the assumption that sum of 2 or many stars is star. (not sure?) 
            // it won't support * ups and downs more than 9
            if (SubgameVal!=VAL_UNK) {
                int SubgameVal_simplified = simplifySumValue(SubgameVal);
                gameVal+= SubgameVal_simplified; 
            }
            
               //should be completed later : incorporate less than zero rule -> for now just says we can't get the value
            else {optionValues.push_back(VAL_UNK); No_value=true;  return VAL_UNK; } // if no value then we cannot get a value for this game
        }
        // gameVal = Subgameval1 + Subgameval2
        optionValues.push_back(gameVal);

        }

        state->undo(undoBuffer);

    }
        
        int return_value = -1; // -> gets replaced fast

                // if black return max lef
            if(player==1){
                return_value = optionValues[0];
                for (int i =1; i<optionValues.size(); i++){
                    // cout<<"return value B "<<return_value << "\n";
                    // implement a way to compare values
                    return_value = compare(optionValues[i],return_value,true);
                    if (return_value== VAL_UNK) return VAL_UNK;

                }

            }
            else{// if white return min right
                return_value = optionValues[0];
                for (int i =1; i<optionValues.size(); i++){
                    // cout<<"return value W "<<return_value << "\n";
                    // implement a way to compare values
                    return_value = compare(optionValues[i],return_value,false);
                    if (return_value== VAL_UNK) return VAL_UNK;
                }
            }
        


    return return_value;
}

void printBits(int x, int length) {
    for (int i = 0; i < length; i++) {
        cout << ((x >> i) & 1);
    }
    cout << endl;
}

int main() {

    Database db;
    //db.load(); //might be wrong to load here

    int maxLength = DB_MAX_BITS;
    int maxGame = 0;

 /*   // Initial patterns saving starts
    string s1 = "";
    string s2 = "";

                
//BLACK = 0
vector<string> string_list;


    for (int m = 2; m < DB_MAX_BITS/2; m++) {
            
        for (int n = 2; n < DB_MAX_BITS/2; n++) {
                s1 = string(m, 'B') + string(n, 'W');
                s2 = string(m, 'W') + string(n, 'B');
                string_list.push_back(s1);
                string_list.push_back(s2);
                
            }
    }
    
    bool found = false;
    string s ="";
    for (int length = 1; length <= maxLength; length++) {
        maxGame *= 2;
        maxGame += 1;

        char boardText[length + 1];
        char board[length + 1];
        char mirrorBoard[length + 1];

        boardText[length] = 0;
        board[length] = 0;
        mirrorBoard[length] = 0;

        unsigned char *entry;

        for (int game = 0; game <= maxGame; game++) {
            s = "";
            bool mirror = false;

            if (game > (maxGame + 2) / 2) {
                mirror = true;
            }

            for (int i = 0; i < length; i++) {
                if ((game >> i) & 1) {
                    boardText[i] = 'W';
                    
                    board[i] = WHITE;
                    s += 'W';
                } else {
                    boardText[i] = 'B';
                    board[i] = BLACK;
                    s += 'B';
                }
                
            }

            entry = db.get(length, board);
            
            if (mirror) {
                for (int i = 0; i < length; i++) {
                    if ((game >> i) & 1) {
                        mirrorBoard[i] = BLACK;
                        
                    } else {
                        mirrorBoard[i] = WHITE;
                    }
                }
                entry = db.get(length, mirrorBoard);
            }
            
            found = (std::find(string_list.begin(), string_list.end(), s) != string_list.end());
            
            if (found == true){
                DB_SET_VALUE(entry, 0);
                continue;

            }

            uint64_t gValue = VAL_UNK;
            gValue = get_pattern_value(boardText, length);
            // cout<<gValue<<"\tGVALUE\n";
            DB_SET_VALUE(entry, gValue);
            // for (int i = 0; i < length; i++) {
            //     cout << boardText[i];
            // }cout << endl;
            // cout<<gValue<<"\tGVALUE\n";
        
    }

    }
    // return 0;
*/
    // initial pattern saving ends

    // cout<<"END \n\n\n\n";
    maxLength = DB_MAX_BITS;
    maxGame = 0;
    
    for (int length = 1; length <= maxLength; length++) {

        maxGame *= 2;
        maxGame += 1;

        char boardText[length + 1];
        char board[length + 1];
        char mirrorBoard[length + 1];
        char mirrorBoardText[length + 1];

        boardText[length] = 0;
        board[length] = 0;
        mirrorBoard[length] = 0;

        unsigned char *entry;

        for (int game = 0; game <= maxGame; game++) {
            // cout << "Length - Game: " << length << " " << game << endl;
            // printBits(game, length);


            bool mirror = false;

            if (game > (maxGame + 2) / 2) {
                mirror = true;
            }

            //mirror = false;

            for (int i = 0; i < length; i++) {
                if ((game >> i) & 1) {
                    boardText[i] = 'W';
                    board[i] = WHITE;
                } else {
                    boardText[i] = 'B';
                    board[i] = BLACK;
                }
            }

            if (mirror) {
                for (int i = 0; i < length; i++) {
                    if ((game >> i) & 1) {
                        mirrorBoard[i] = BLACK;
                        mirrorBoardText[i] = 'B';
                    } else {
                        mirrorBoard[i] = WHITE;
                        mirrorBoardText[i] = 'W';
                    }
                }
            }


            int outcome = 0;
            uint64_t domBlack = 0;
            uint64_t domWhite = 0;
            uint64_t gameValue_mirror = 0;

            if (mirror) {
                entry = db.get(length, mirrorBoard);
                outcome = DB_GET_OUTCOME(entry);
                domBlack = DB_GET_DOMINATED(entry, 1);
                domWhite = DB_GET_DOMINATED(entry, 2);
                // game_values
                gameValue_mirror = DB_GET_VALUE(entry);
                // cout<< "original game value: "<< gameValue_mirror<<"\n";

                switch (outcome) {
                    case OC_B:
                        outcome = OC_W;
                        break;

                    case OC_W:
                        outcome = OC_B;
                        break;

                    case OC_N:
                        outcome = OC_N;
                        break;

                    case OC_P:
                        outcome = OC_P;
                        break;
                }


                entry = db.get(length, board);
                DB_SET_OUTCOME(entry, outcome);
                DB_SET_DOMINATED(entry, 1, domWhite); //invert dominance
                DB_SET_DOMINATED(entry, 2, domBlack);

                //invert game values -> exchange ups and downs; stars remain the same
                if (gameValue_mirror!=VAL_UNK){
                    int ups =  int(gameValue_mirror/100);
                    int downs = int((gameValue_mirror/10)%10);
                    int stars = int(gameValue_mirror%10);
                    uint64_t InverseValue = downs*100 + ups*10 + stars;

                    // cout<< "mirror game value: "<< InverseValue<<"\n";
                    DB_SET_VALUE(entry,InverseValue);
                }
                else DB_SET_VALUE(entry,VAL_UNK);

                    // printing the final gameValue       
                    unsigned int mir = DB_GET_VALUE(entry);
                    if (mir!=VAL_UNK){
                        for (int i = 0; i < length; i++) {
                            if (mirrorBoardText[i]=='W'){
                                std::cout<<'B';}
                            else std::cout<<'W';
                        }
                        cout << endl;
                        cout<<mir<<"\t mirror game value\n";
        
                    }
                continue;
            }


            // cout << "solving" << endl;
            int result1, result2;

            {
                auto start = std::chrono::steady_clock::now();
                BasicSolver *solver = new BasicSolver(1, length, &db);
                solver->timeLimit = 1000000000.0;
                solver->startTime = start;

                State *root = new State(boardText, 1);
                
                result1 = solver->solveID(root, 1, opponentNumber(1));

                delete solver;
                delete root;
            }

            {
                auto start = std::chrono::steady_clock::now();
                BasicSolver *solver = new BasicSolver(2, length, &db);
                solver->timeLimit = 1000000000.0;
                solver->startTime = start;

                State *root = new State(boardText, 2);
                
                result2 = solver->solveID(root, 2, opponentNumber(2));

                delete solver;
                delete root;
            }

            outcome = 0;

            if (result1 == result2) {
                outcome = result1;
            } else if (result1 == 1) {
                outcome = OC_N;
            } else {
                outcome = OC_P;
            }

            // std::cout << "Lookup" << std::endl;
            entry = db.get(length, board);
            if (DB_GET_OUTCOME(entry) != 0) {
                // cout << "Overwriting outcome: " << DB_GET_OUTCOME(entry) << endl;
                while(1){}
            }
            // uint64_t a = 0;
            DB_SET_OUTCOME(entry, outcome);              


            if (length <= DB_MAX_DOMINANCE_BITS ) {
                //find dominated moves for B
                char sumBoard[length + 1 + length + 1];
                sumBoard[length + 1 + length] = 0;
                sumBoard[length] = E;

                {
                    State s1(boardText, 1);
                    State s2(boardText, 1);

                    char boardCopy[length];
                    memcpy(boardCopy, s2.board, length);

                    size_t moveCount = 0;
                    int *moves = s1.getMoves(1, 2, &moveCount);

                    char undo1[sizeof(int) + 2 * sizeof(char)];
                    char undo2[sizeof(int) + 2 * sizeof(char)];

                    for (int i = 0; i < moveCount; i++) {
                        s1.play(moves[2 * i], moves[2 * i + 1], undo1);

                        for (int j = i + 1; j < moveCount; j++) {
                            s2.play(moves[2 * j], moves[2 * j + 1], undo2);

                            memcpy(sumBoard, s1.board, length);
                            memcpy(&sumBoard[length + 1], s2.board, length);


                            for (int k = length + 1; k < 2 * length + 1; k++) {
                                sumBoard[k] = opponentNumber(sumBoard[k]);
                            }


                            int bFirst = gameResult(db, sumBoard, 2 * length + 1, 1);
                            int wFirst = gameResult(db, sumBoard, 2 * length + 1, 2);

                            memcpy(s2.board, boardCopy, length);

                            if (bFirst == wFirst) {
                                if (bFirst == 1) { // I - J is positive for black --> I > J
                                    domBlack |= (((uint64_t) 1) << j);
                                } else { // I - J is negative for black --> I < J
                                    domBlack |= (((uint64_t) 1) << i);
                                }
                                //while (1) {}
                            }



                            //s2.undo(undo2);
                        }

                        s1.undo(undo1);
                    }

                    if (moveCount > 0) {
                        delete[] moves;
                    }
                }

                //find dominated moves for W
                {
                    State s1(boardText, 2);
                    State s2(boardText, 2);

                    char boardCopy[length];
                    memcpy(boardCopy, s2.board, length);

                    size_t moveCount = 0;
                    int *moves = s1.getMoves(2, 1, &moveCount);

                    char undo1[sizeof(int) + 2 * sizeof(char)];
                    char undo2[sizeof(int) + 2 * sizeof(char)];

                    for (int i = 0; i < moveCount; i++) {
                        s1.play(moves[2 * i], moves[2 * i + 1], undo1);

                        for (int j = i + 1; j < moveCount; j++) {
                            s2.play(moves[2 * j], moves[2 * j + 1], undo2);

                            memcpy(sumBoard, s1.board, length);
                            memcpy(&sumBoard[length + 1], s2.board, length);


                            for (int k = length + 1; k < 2 * length + 1; k++) {
                                sumBoard[k] = opponentNumber(sumBoard[k]);
                            }


                            int bFirst = gameResult(db, sumBoard, 2 * length + 1, 1);
                            int wFirst = gameResult(db, sumBoard, 2 * length + 1, 2);

                            memcpy(s2.board, boardCopy, length);


                            if (bFirst == wFirst) {
                                if (bFirst == 2) { // I - J is positive for white --> I > J
                                    domWhite |= (((uint64_t) 1) << j);
                                } else { // I - J is negative for white --> I < J
                                    domWhite |= (((uint64_t) 1) << i);
                                }
                                //while (1) {}
                            }



                            //s2.undo(undo2);
                        }

                        s1.undo(undo1);
                    }

                    if (moveCount > 0) {
                        delete[] moves;
                    }
                }


                DB_SET_DOMINATED(entry, 1, domBlack);
                DB_SET_DOMINATED(entry, 2, domWhite);
                // std::cout << domBlack << " " << domWhite << std::endl;
            }
            //end of dominated


            // if(DB_GET_VALUE(entry)==VAL_UNK){
                //start of game value for left and right
                if(length>=2){
                // first check if it is one of the three patterns and has already a value
                //
                // IF not:
                //calculate game values for R and L on this board
                    int Black_value = gameValue(db, board, length, 1);
                    int White_value = gameValue(db, board, length, 2);
                    // cout<< "B value" <<Black_value<<"white:"<< White_value << "\n";

                    if (Black_value!=VAL_UNK & White_value!=VAL_UNK){
                        uint64_t board_value = set_basic_value_rules(Black_value,White_value);
                        DB_SET_VALUE(entry, board_value);
                        //printing
                        if(board_value!=VAL_UNK){

                            for (int i = 0; i < length; i++) {
                            cout << boardText[i];
                            }cout << endl;
                            cout<< "board value R and L \n" <<board_value<<"\n";
                        
                        }
                    }
                    else{
                    DB_SET_VALUE(entry, VAL_UNK); 
                    }

                } else{
                    DB_SET_VALUE(entry, 0);
                }
        // }
        // else { // if it is solved by patterns
        //     // printing the final gameValue       
        //     a = DB_GET_VALUE(entry);
        //     if (a!=VAL_UNK){

        //         for (int i = 0; i < length; i++) {
        //         cout << boardText[i];
        //         }
        //         cout << endl;
        //         cout<<a<<"\t PATTERN GAME VALUE\n";
        //     }
        // }

            // cout << endl;
        }

    }


    db.save();
    
    return 0;
}
