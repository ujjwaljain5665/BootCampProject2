//SUDOKU---Ujjwal Jain_19011313
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class subSquare{
private:
    //fNum is the final assigned number, pNums are numbers that a subsquare could potentially be.
    int fNum;
    vector<int> pNums;
public:
    void assignFNum(int const& num){
        pNums.clear();
        fNum=num;
    }
    void assignPNum(int const& num){
        pNums.push_back(num);
    }
    int getFNum(){
        return fNum;
    }
    vector<int> getPNums(){
        return pNums;
    }
    void removePNum(int const& num){
        int pos = (distance(pNums.begin(), find(pNums.begin(), pNums.end(), num)));
        pNums.erase(pNums.begin() + pos);
    }
    bool PNumsContain(int const& num){
        return(find(pNums.begin(), pNums.end(), num) < pNums.end());
    }
};

//Used to make sure the user is only entering rows of numbers.
bool isNumeric(string const& input){
    for(int itr=0; itr<input.length(); itr++){
        if(!isdigit(input.at(itr))){
            return false;
        }
    }
    return true;
}

//Because I'll be damned if I'm gonna write a program that crashes when the user types "soLvE" or "BacK".
void strToUpper(string& input){
    for(int itr=0; itr<input.length(); itr++){
        input[itr]=toupper(input[itr]);
    }
}

//Reads the Sudoku grid. Name says it all.
void readGrid(subSquare input[9][9]){
    for(int row=0; row<9; row++){
        for(int col=0; col<9; col++){
            printf("%d ", input[row][col].getFNum());
        }
        printf("\n");
    }
}

//User enters the Sudoku row by row. Allows for mess-up correction.
void createGrid(subSquare input[9][9]){
    string currentRow;
    printf("Enter the grid below, row by row. put zeroes where nothing is assigned. No delimiting characters. Enter 'back' to go back one row if you messed up.\n");
    int row=0;
    while(row<9){
        currentRow="";
        printf("Enter row %d:\n", row+1);
        getline(cin, currentRow);
        strToUpper(currentRow);
        if(isNumeric(currentRow)){
            if(currentRow.length()==9){
                for(int col=0; col<9; col++){
                    input[row][col].assignFNum(currentRow.at(col) - '0');
                }
                row++;
            }else if(currentRow.length() < 9){
                printf("ERROR: entry too short\n");
            }else{
                printf("ERROR: entry too long\n");
            }
        }else{
            if(currentRow=="BACK"){
                switch(row){
                    case 0: printf("Cannot go back further\n"); break;
                    default: row--; break;
                }
            }else{
                printf("ERROR: entry not valid\n");
            }
        }
    }
}

//Copies one grid of sudoku subsquares to another. For use in the event the state of one 2D grid needs saved.
void cpyGrid(subSquare dst[9][9], subSquare src[9][9]){
    for(int row = 0; row < 9; row++){
        for(int col = 0; col < 9; col++){
            dst[row][col] = src[row][col];
        }
    }
}

//Needed to make this one to use sets of sudoku grids. Mode denotes whether or not we're adding to a set of grids (TD) or copying one from a set of grids to another individual 2D grid (arr, couldn't think of a decent name).
void cpyGrid2(subSquare TD[9][9][9], subSquare arr[9][9], int const& TDIndx, string const& mode){
    if(mode == "TO_3D"){
        for(int row = 0; row < 9; row++){
            for(int col = 0; col < 9; col++){
                TD[TDIndx][row][col] = arr[row][col];
            }
        }
    }else if(mode == "TO_2D"){
        for(int row = 0; row < 9; row++){
            for(int col = 0; col < 9; col++){
                arr[row][col] = TD[TDIndx][row][col];
            }
        }
    }
}

//Checks if there are any 0s (i.e. unassigned subsquares) in the grid passed in.
bool gridUnsolved(subSquare input[9][9]){
    for(int row = 0; row < 9; row++){
        for(int col = 0; col < 9; col++){
            if(input[row][col].getFNum() == 0){
                return true;
            }
        }
    }
    return false;
}

//For checking the relevant 3x3 square. Indx vars are the subsquare in question, PH means PlaceHolder (i.e. the row/column where the upper-left-most subsquare in the 3x3 square is.
void mkSqrVars(int const& rowIndx, int const& colIndx, int& rowPH, int& colPH){
    switch(rowIndx){
        case 0: case 1: case 2:
            rowPH=0;
            break;
        case 3: case 4: case 5:
            rowPH=3;
            break;
        case 6: case 7: case 8:
            rowPH=6;
            break;
        default: break;
    }
    switch(colIndx){
        case 0: case 1: case 2:
            colPH=0;
            break;
        case 3: case 4: case 5:
            colPH=3;
            break;
        case 6: case 7: case 8:
            colPH=6;
            break;
        default: break;
    }
}

/*Quick note about the (x)ContainsNum functions when they're looking for pNums: they return true when instances exceeds 1 because if a number could potentially be assigned to a
part of the grid, it will have one (but ONLY one) instance in that part of the grid.*/

//Checks if a 3x3 square contains an fNum or pNums. Does this by checking the subsquare that mkSqrVars tells it to for the fNum.
bool squareContainsNum(subSquare input[9][9], int const& rowIndx, int const& colIndx, int const& num, string const& checkingFor){
    int instances = 0;

    int row;
    int rowPH;
    int col;
    int colPH;
    mkSqrVars(rowIndx, colIndx, rowPH, colPH);
    //stopRow/Col: the row or col to stop at (i.e. the place where the relevant 3x3 square ends.
    int stopRow=rowPH+3;
    int stopCol=colPH+3;
    if(checkingFor == "fNums"){
        for(row=rowPH; row<stopRow; row++){
            for(col=colPH; col<stopCol; col++){
                if(input[row][col].getFNum() == num){
                    return true;
                }
            }
        }
        return false;
    }else if(checkingFor == "pNums"){
        for(row = rowPH; row < stopRow; row++){
            for(col = colPH; col < stopCol; col++){
                if(input[row][col].PNumsContain(num)){
                    instances++;
                    if(instances > 1){
                        return true;
                    }
                }
            }
        }
        return false;
    }
}

//Checks if the row contains an fNum or pNums.
bool rowContainsNum(subSquare input[9][9], int const& rowIndx, int const& num, string const& checkingFor){
    int instances = 0;
    if(checkingFor == "fNums"){
        for(int col = 0; col < 9; col++){
            if(input[rowIndx][col].getFNum() == num){
                return true;
            }
        }
        return false;
    }else if(checkingFor == "pNums"){
        for(int col = 0; col < 9; col++){
            if(input[rowIndx][col].PNumsContain(num)){
                instances++;
                if(instances>1){
                    return true;
                }
            }
        }
        return false;
    }
}

//Checks if the column contains an fNum or pNums.
bool colContainsNum(subSquare input[9][9], int const& colIndx, int const& num, string const& checkingFor){
    int instances = 0;
    if(checkingFor == "fNums"){
        for(int row = 0; row < 9; row++){
            if(input[row][colIndx].getFNum() == num){
                return true;
            }
        }
        return false;
    }else if(checkingFor == "pNums"){
        for(int row = 0; row < 9; row++){
            if(input[row][colIndx].PNumsContain(num)){
                switch(instances){
                    case 0: instances++; break;
                    default: return true; break;
                }
            }
        }
        return false;
    }
}

//Assigns an fNum to a subSqaure. This means removing the pNums matching the fNum being assigned from the adjescent row, column, and 3x3 square.
void fullAssign(subSquare input[9][9], int const& rowIndx, int const& colIndx, int const& num){
    input[rowIndx][colIndx].assignFNum(num);

    //Erase number from pNums of subsquares where it's no longer applicable: row
    for(int c = 0; c < 9; c++){
        if(input[rowIndx][c].PNumsContain(num)){
            input[rowIndx][c].removePNum(num);
        }
    }
    //Erase number from pNums of subsquares where it's no longer applicable: col
    for(int r = 0; r < 9; r++){
        if(input[r][colIndx].PNumsContain(num)){
            input[r][colIndx].removePNum(num);
        }
    }
    //Erase number from pNums of subsquare where it's no longer applicable: square
    int row;
    int rowPH;
    int col;
    int colPH;
    mkSqrVars(rowIndx, colIndx, rowPH, colPH);
    int stopRow = rowPH+3;
    int stopCol = colPH+3;
    for(row = rowPH; row < stopRow; row++){
        for(col = colPH; col < stopCol; col++){
            if(input[row][col].PNumsContain(num)){
                input[row][col].removePNum(num);
            }
        }
    }
}

//Assigns pNums based on what fNums are already assigned.
void phase1(subSquare input[9][9]){
    for(int row = 0; row < 9; row++){
        for(int col = 0; col < 9; col++){
            for(int num = 1; num < 10; num++){
                //Pushes num to pNums if it's not already an fNum in the subSquare's row/col/3x3 square.
                if(input[row][col].getFNum() == 0){
                    if(!squareContainsNum(input, row, col, num, "fNums") &&
                     !rowContainsNum(input, row, num, "fNums") &&
                     !colContainsNum(input, col, num, "fNums")){
                       input[row][col].assignPNum(num);
                    }
                }
            }
        }
    }
}

//Assigns fNums based on what the pNums will allow. Does this over and over as long as numbers keep getting assigned. Not all Sudokus will require phase3, this solves most easy ones.
void phase2(subSquare input[9][9]){
    int numbersAssigned = 0;
    do{
        numbersAssigned = 0;
        for(int row = 0; row < 9; row++){
            for(int col = 0; col < 9; col++){
                for(int num = 1; num < 10; num++){
                    //Does a full assign if the subSquare is unassigned, could be num, and is the only one that could be num in at least its row/col/3x3 square.
                    if(input[row][col].getFNum() == 0 && input[row][col].PNumsContain(num)){
                        if( (input[row][col].getPNums().size() == 1) ||
                            (!squareContainsNum(input, row, col, num, "pNums") || 
                             !rowContainsNum(input, row, num, "pNums") || 
                             !colContainsNum(input, col, num, "pNums"))){
                                fullAssign(input, row, col, num);
                                numbersAssigned++;
                        }
                    }
                }
            }
        }
    }while(numbersAssigned);
}

/*For when phase2 stops but there are still 0s. This occurs when the pNums can no longer be used alone to decide what should be assigned where, which means we've got to start guessing.
This is where it's either solved or deemed impossible to solve (by the standards of the program, at least...).*/
void phase3(subSquare input[9][9]){
    //Save the state of the subSquare to revert in the event of an incorrect guess.
    subSquare savedState[9][9];
    cpyGrid(savedState, input);

    //Row and column for where we're going to start guessing.
    int guessRow;
    int guessCol;

    //Finds the first zero to start guessing on.
    for(int row = 0; row < 9; row++){
        for(int col = 0; col < 9; col++){
            if(input[row][col].getFNum() == 0){
                guessRow = row;
                guessCol = col;
                row = 10;
                col = 10;
            }
        }   
    }

    //The number of guesses to be made (i.e. pNums in the chosen subSquare).
    int guesses = savedState[guessRow][guessCol].getPNums().size();

    //The number with which the guess is being made.
    int guessNum;
    //The number of guesses that have been made.
    int guessNumIndx = 0;
    //An array of 9x9 grids, for when we need to make more than one guess. This rarely happens, but it still happens.
    subSquare TD[9][9][9];
    //Number of grids produced as a result of guessing.
    int numGuessGrids = 0;

    if(input[guessRow][guessCol].getPNums().size() > 0){
        while(guessNumIndx < guesses && gridUnsolved(input)){
            cpyGrid(input, savedState);
            guessNum = input[guessRow][guessCol].getPNums()[guessNumIndx];
            fullAssign(input, guessRow, guessCol, guessNum);
            phase2(input);
            if(gridUnsolved(input)){
                //If that guess didn't work, then we just try the next one.
                cpyGrid2(TD, input, guessNumIndx, "TO_3D");
                numGuessGrids++;
                guessNumIndx++;
            }else{
                guessNumIndx = guesses+1;
            }
            if(guessNumIndx == guesses){
                /*If we've gone through every guess and none of them solved it, but it's still solveable, one of them MUST be right. So we have to revert to the first guess, and then make guesses on the zero after THAT one, repeating the process
                until it's made all the correct guesses. Basically goes through every possible guessing path until it finds the right one.*/
                int i = 0;
                subSquare ss[9][9];
                cpyGrid(ss, savedState);
                while(gridUnsolved(ss) && i < numGuessGrids){
                    cpyGrid2(TD, ss, i, "TO_2D");
                    phase3(ss);
                    i++;
                }
                cpyGrid(input, ss);
            }
        }
    }
}

void solve(subSquare grid[9][9]){
    printf("Before solving: \n");
    readGrid(grid);
    phase1(grid);
    phase2(grid);
    if(gridUnsolved(grid)){
        phase3(grid);
    }
    if(!gridUnsolved(grid)){
        printf("After solving:\n");
        readGrid(grid);
    }else{
        printf("ERROR: Impossible to solve. Closest it got:\n");
        readGrid(grid);
    }
}

int main(){
    printf("Enter 'gen' or 'solve'\n");
    string str;
    cin >> str;
    cin.ignore();
    strToUpper(str);
    if(str=="SOLVE"){
        subSquare grid[9][9];
        createGrid(grid);
        solve(grid);
    }else if(str=="GEN"){
        //This comes next
    }else{
        printf("ERROR: invalid option entered\n");
        return 1;
    }
    return 0;
}