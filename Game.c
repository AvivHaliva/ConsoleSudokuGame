
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mainAux.h"
#include "Game.h"
#include "Solver.h"



/***** Memory: allocation & free *****/

/*
 * allocate memory for the given board
 * in size of N*N
 * in case of error - print message to user and exit the game
 * NOTE: no need to exit cleanly on errors
 */
void allocateboardMem (int *** board, int N){
	int i;

	/* allocate mem for board rows */
	*board = malloc(N*(sizeof(int*)));
	if (*board == NULL){
		printMemoryAllocationError();
		exit(0);
	}
	/* allocate mem for board cols */
	for (i=0; i < N; i++){
		(*board)[i] = malloc(N*sizeof(int));
		if ((*board)[i] == NULL){
			printMemoryAllocationError();
			exit(0);
		}
	}
}

/*
 * free the memory of the given board in size NXN
 */
void freeBoardMem (int ** board, int N){
	int i ;
	for (i = 0; i < N; i++)
	{
	    free(board[i]);
	}
	free(board);
}

/*
 * free all the game boards allocated memory
 */
void freeGameBoards (Game * game){
	freeBoardMem(game->gameBoard, game->N);
	freeBoardMem(game->solvedBoard, game->N);
	freeBoardMem(game->originalBoard, game->N);
}



/***** Initialization *****/

void initMovesList(Game * game) {
	game->moves = (LinkedList*) malloc(sizeof(LinkedList));
	game->moves->head = NULL;
	game->moves->tail = NULL;
	game->moves->movePointer = NULL;
}

/*
 * gets game, m and n
 * Initialize boards dimension and allocate memory for boards array
 */
void initalizeGameBoards(Game * game, int m, int n){
	/*
	 * initializing boards dimension
	 */
	game->n = n;
	game->m = m;
	game->N = n*m;

	/*
	 * allocate mem for boards
	 */
	allocateboardMem (&(game->gameBoard), game->N);
	allocateboardMem (&(game->originalBoard), game->N);
	allocateboardMem(&(game->solvedBoard), game->N);
}

void initalizeGameAtINIT(Game* game, int initializeMarkErrors){
	game->gameMode = INIT_MODE;
	if(initializeMarkErrors == 1) {
		game->markErrors = 1;
	}
	game->gameOver = 0;
	game->gameBoard = NULL;
	game->originalBoard = NULL;
	game->solvedBoard = NULL;
	initMovesList(game);
	game->isErroneous = 0;
	game->isSolvable = 0;
}



/***** Files: read & write (internal functions) *****/

/*
 * gets file pointer and pointer to the game
 * read m,n and cell values from the file
 * and initialize the boards with those values
 *
 * NOTE: the function can deal with any type and amount of spaces -
 * they won't be read into the boards
 */
void readBoardValuesFromFile (FILE * fp, Game * game){

	/* booleans */
	int readm = 0; /* 1 iff we read m */
	int readn = 0; /* 1 iff we read n */
	int done = 0;	/* 1 iff we read all the board cell */

	char * line;
	char * parsedLine;

	int n;
	int m;

	/*readCellCnt counts the number of board cells that we read from the file*/
	int readCellCnt = 0;
	int i;
	int j;

	line = getLine(fp);
	/* read all lines and split each line according to delimiter */
	while (line != NULL){
		parsedLine = strtok(line, DELIMITER);
		while (parsedLine != NULL){
			/* first read m */
			if (readm == 0){
				m = atoi (parsedLine);
				readm = 1;
			}
			/* then read n */
			else if (readn == 0){
				n = atoi (parsedLine);
				readn = 1;
				initalizeGameBoards(game, m, n);
			}
			/* if we already read m and m - read board cells values */
			else{
				i = readCellCnt / game->N;
				j = readCellCnt % game->N;

				game->gameBoard[i][j] = atoi(parsedLine);
				/* 1. in Edit mode - none of the cells is loaded as Fixed cell
				 * 2. only fixed cells are loaded to original board
				 */
				if (game->gameMode != EDIT_MODE &&
						strstr(parsedLine,FIXED_CELL_DOT) !=  NULL){
					game->originalBoard[i][j] = game->gameBoard[i][j];
				}
				else{
					game->originalBoard[i][j] = 0;
				}

				readCellCnt++;
				/* if we read N*N cell values from the file - change done to 1
				 * and then get out of the loop*/
				if(readCellCnt == game->N * game->N) {
					done = 1;
					break;
				}
			}
			parsedLine = strtok(NULL, DELIMITER);
		}
		free(line);
		if(done == 1) {
			return;
		}
		line = getLine(fp);
	}
}


/*
 * Gets game, file path and the command type
 * reads board values from the given filePath (if exists)
 * and loads them to the game board
 */
int getBoardFromFile(Game * game, char * path, commandType type){
	FILE *fp;

	/*
	 * open the file.
	 * if the operation fails - prints error and the command isn't executed
	 */
	fp = fopen(path, "r");
	if (fp == NULL){

		/* if there is a problem with file access -
		 * print message to user (message content depends on command type)
		 * and don't execute the command
		 */
		if (type == SOLVE){
			printSolveFileAccessError();
		}
		else{
			/* the requested command is edit */
			printEditFileAccessError();
		}

		return 0;
	}

	/* free previous boards and moves list */
	/* if there is another boards loaded to the game - erase it */
	if(game->gameBoard != NULL){
		freeGameBoards(game);
	}
	freeLinkedList(game->moves);

	/* fill the game board and the original board with the given file values */
	readBoardValuesFromFile(fp, game);

	/* close the file and make sure the stream was successfully closed */
	if (fclose(fp) != 0){
		printFileCloseError();
		exit(0);
	}
	return 1;
}

/*
 * Save board values from game entity to a given file .
 *
 * The format of the file is as follows (a simple text file):
 * 1. The first line contains the block size m n.
 * 	  These are two integers, separated by a single space.
 * 2. Each line contains a single row of the board.
 * 3. Each cell contains the cell's value separated with single spaces.
 *  	i.  the value in empty cells is 0.
 * 		ii. if the cell is "fixed", follow its value with a dot '.'
 *
 * The function return value is 1 if the writing succeed,
 * but if there was a problem - the return value is 0
*/

int saveBoardToFile (Game game,  FILE * fp){
	int i;
	int j;

	/* print m an n to the file */
	fprintf(fp,"%d%s%d%s",game.m, SPACE, game.n, NEW_LINE);

	/* write cells value to the file line by line according to format rules */
	for (i = 0; i< game.N; i++){
		for (j = 0; j< game.N; j++){
			fprintf(fp,"%d",abs(game.gameBoard[i][j]));
			/* mark cell as Fixed:
			 * in case the cell is Fixed or we're in Edit mode */
			if ((game.gameMode == EDIT_MODE && game.gameBoard [i][j] !=0)
					|| game.originalBoard[i][j] != 0){
				fprintf(fp,"%s", FIXED_CELL_DOT);
			}
			if (j < game.N-1) {
				fprintf(fp,"%s", SPACE);
			}
		}
		fprintf(fp,"%s", NEW_LINE);
	}
	return 1;
}



/***** Errors: check & mark board accordingly *****/

int checkAndMarkBoardErrorsInRow(Game * game, int i, int j, int mark) {
	int k;
	int foundError = 0;
	int val = abs(game->gameBoard[i][j]);

	for (k = 0; k < game->N; k++) {
		if(k != j && abs(game->gameBoard[i][k]) == val) {
			if (mark == 1){
				game->gameBoard[i][k] = val * (-1);
			}
			foundError = 1;
		}
	}

	if(foundError == 1 && mark == 1) {
		game->gameBoard[i][j] = val * (-1);
	}
	return foundError;
}

int checkAndMarkBoardErrorsInCol(Game * game, int i, int j, int mark) {
	int k;
	int foundError = 0;
	int val = abs(game->gameBoard[i][j]);

	for (k = 0; k < game->N; k++) {
		if(k != i && abs(game->gameBoard[k][j]) == val) {
			if (mark == 1){
				game->gameBoard[k][j] = val * (-1);
			}
			foundError = 1;
		}
	}

	if(foundError == 1 && mark == 1) {
		game->gameBoard[i][j] = val * (-1);
	}
	return foundError;
}

int checkAndMarkBoardErrorsInBlock(Game * game, int i, int j , int mark) {
	int r;
	int c;
	int foundError = 0;
	int val = abs(game->gameBoard[i][j]);
	int startRow = (i/game->m)*game->m;
	int startCol = (j/game->n)*game->n;

	for (r = startRow ; r < startRow + game->m; r++) {
		for (c = startCol; c < startCol + game->n ; c++){
			if (r == i && c == j){
				continue;
			}
			if (abs(game->gameBoard[r][c]) == val){
				if (mark == 1){
					game->gameBoard[r][c] = val * (-1);
				}
				foundError = 1;
			}
		}
	}

	if(foundError == 1 && mark == 1) {
		game->gameBoard[i][j] = val * (-1);
	}
	return foundError;
}

/*
 * go over the board up-down, left to right,
 * and set all of the cells values to positive
 */
void setBoardPositive(Game * game) {
	int i;
	int j;

	for (i = 0; i < game->N; i++){
		for (j = 0; j < game->N; j++){
			game->gameBoard[i][j] = abs(game->gameBoard[i][j]);
		}
	}
}

/*
 * Go over the game board up-down, left to right.
 * for each cell <i,j>, check if there is an error in the cell row (i),
 * col (j) and block. if an error was found - the cell is marked as negative
 * (mark flag = 1), as explained in Game.h.
 *
 * The function updates isErroneous value to 1 if an error was found.
 */
void checkAndMarkBoardErros(Game * game){
	int i;
	int j;
	int foundError = 0;

	setBoardPositive(game);

	for (i = 0; i < game->N; i++){
		for (j = 0; j < game->N; j++){
			if (game->gameBoard[i][j] != 0){
				foundError += checkAndMarkBoardErrorsInRow(game, i, j,1);
				foundError += checkAndMarkBoardErrorsInCol(game, i, j,1);
				foundError += checkAndMarkBoardErrorsInBlock(game, i, j,1);
			}
		}
	}

	if(foundError != 0) {
		game->isErroneous = 1;
	}
	else {
		game->isErroneous = 0;
		setBoardPositive(game);
	}
}

/*
 * gets a pointer to game, the cell new value and the cell old value.
 * the function mark and check errors in the board game after the change,
 * and also update numOfNonZeroCells in the following cases:
 * 1. if it is a clear action (newZ = 0) and the cell
 * 		wasn't empty before the change - update numOfNonZeroCells--.
 * 2. if it is a regular set action (newZ != 0) and the cell
 * 		was empty before the change - update numOfNonZeroCells++.
 */
void updateErrorsAndEmptyCellNumAfterCellValueChange(Game * game, int newZ, int oldZ){

	/* If this is a clear action */
	if (newZ == 0) {
		checkAndMarkBoardErros(game);
		if(oldZ != 0) {
			game->numOfNonZeroCells--;
		}
	}
	/* Normal set action*/
	else {
		checkAndMarkBoardErros(game);
		if (oldZ == 0) {
			game->numOfNonZeroCells++;
		}
	}
}



/***** Auxiliary functions for user commands *****/

/*
 * gets an int x and a range.
 * return 1 iff x is in the requested range.
 */
int inputInRange(int x, int rangeStart, int rangeEnd) {
    if (x >= rangeStart && x <= rangeEnd) {
        return 1;
    }
    return 0;
}

/*
 * generate an empty 9*9 boards

 * fill all gameBoard and originalBoard cells with zeros
 * update isErroneous and numOfNonZeroCells values to zero
 */
void genEmptyBoard(Game * game){
	int i;
	int j;

	/* free previous boards and moves list */
	/* if there is another boards loaded to the game - erase it */
	if(game->gameBoard != NULL){
		freeGameBoards(game);
	}
	freeLinkedList(game->moves);

	initalizeGameBoards(game, DEFAULT_BOARD_n, DEFAULT_BOARD_m);
	for (i = 0; i < game->N; i++){
		for (j = 0; j< game->N; j++){
			game->gameBoard[i][j] = 0;
			game->originalBoard[i][j] = 0;
		}
	}
	/* update isErroneous and numOfNonZeroCells values to zero */
	game->isErroneous = 0;
	game->numOfNonZeroCells = 0;
}

/* get pointer to a game and sets all boards cell to zero */
void clearBoards(Game* game){
	int i;
	int j;

	for (i = 0; i < game->N; i++){
		for (j = 0; j < game->N ; j++){
			game->gameBoard[i][j] = 0;
			game->originalBoard[i][j] = 0;
			game->solvedBoard[i][j] = 0;
		}
	}
}

/*
 * gets a pointer to game and checks if we got to the end of the game:
 * if non of the cells is empty and we are in solve mode:
 * 1.	if the solution isn't valid - print a suitable message
 * 2.	else - the puzzle is solved successfully, so the program prints
 * 		a suitable message, frees game entities (boards and list)
 * 		and initialize the game back to INIT mode.
 */
void checkSolvedSuccessfullyAndFinish(Game * game){
	int N = game->N;

	if (game->gameMode == SOLVE_MODE &&
			game->numOfNonZeroCells == N*N) {

	    if (game->isErroneous == 1) {
	        printPuzzleSolutionErroneous();
	    }
	    else {
	        printPuzzleSolvedSuccessfullyMessage();
	        freeGameBoards(game);
	        freeLinkedList(game->moves);
	        initalizeGameAtINIT(game, 0);
	    }
	}
}

/*
 * return cell <x,y> obvious value if such exists.
 * if there is more than one option (or 0 options) - return 0
 */
int getCellObiviousValue(Game * game,int x,int y){
	int v;
	int obviousVal = 0;
	int N = game->N;

	for (v = 1; v <= N; v++){
		/* try and set cell <x,y> to v */
		game->gameBoard[x][y] = v;

		if (checkAndMarkBoardErrorsInCol(game, x, y, 0) == 0 &&
				checkAndMarkBoardErrorsInRow(game, x, y, 0) == 0 &&
				checkAndMarkBoardErrorsInBlock(game, x, y, 0) == 0){
			if (obviousVal == 0){
				obviousVal = v;
			}
			else {
				game->gameBoard[x][y] = 0;
				return 0;
			}
		}
		game->gameBoard[x][y] = 0;
	}
	return obviousVal;

}

/*
 * gets a pointer to the game and returns the number of non-empty cells
 * in the game board
 */
int cntNumOfNonZeroCell (Game * game){
	int i;
	int j;
	int cnt = 0;

	for (i = 0; i < game->N; i++){
		for (j = 0; j < game->N ; j++){
			if(game->gameBoard[i][j] != 0){
				cnt++;
			}
		}
	}
	return cnt;
}

/*
 * gets a pointer to the game and 2 flags:
 * 1. isUndo - 1 iff the command is undo
 * 2. toPrint - 1 iff printing message to the user is required
 *
 * the function execute the current undo\redo desired move,
 * according to isUndo flag value,
 * including updating the board, the moves list current pointer,
 * the number of nonEmpty cells and board errors accordingly.
 *
 * if toPrint flag is equal to 1,
 * the function also prints the game board
 * and the cell changes that were made in the undo/redo move,
 * in order of execution.
 */
void undoRedoMove(Game * game, int isUndo, int toPrint) {
	int x;
	int y;
	int z;
	int oldZ;
	InnerListNode * currMove;

	if(isUndo == 1) {
		currMove = game->moves->movePointer->move;
	}
	else {
		currMove = game->moves->movePointer->next->move;
	}

	while(currMove != NULL) {
		x = currMove->cellChange[0];
		y = currMove->cellChange[1];
		z = currMove->cellChange[2];
		oldZ = currMove->cellChange[3];
		if(isUndo == 1){
			game->gameBoard[x][y] = oldZ;
			updateErrorsAndEmptyCellNumAfterCellValueChange(game,oldZ,z);
		}
		else {
			game->gameBoard[x][y] = z;
			updateErrorsAndEmptyCellNumAfterCellValueChange(game,z,oldZ);
		}
		currMove = currMove->next;
	}

	if(toPrint == 0) {
		return;
	}

	printBoard(*game);
	if(isUndo == 1) {
		currMove = game->moves->movePointer->move;
	}
	else {
		currMove = game->moves->movePointer->next->move;
	}

	while(currMove != NULL) {
		x = currMove->cellChange[0];
		y = currMove->cellChange[1];
		z = currMove->cellChange[2];
		oldZ = currMove->cellChange[3];
		if(isUndo == 1){
			printUndoRedoMove(x, y, z, oldZ,1);
		}
		else {
			printUndoRedoMove(x, y, z, oldZ,0);
		}
		currMove = currMove->next;
	}

}

/*
 * the function attempts to generate a game board using ILP
 * according to the supplied:
 * 1. x - number of cells to randomly fill
 * 2. y - number of cells not clear
 * 3. cellsArray - an N*N array, so that each array cell represent a game board cell
 * 				   in order to randomly choose cells to fill
 * 4. valuesArray - an N size array, so that each array cell represent a game board
 * 				   legal value (1-N), in order to randomly cell value
 *
 * The function return 1 iff a legal board was generated successfully.
 * If there was a problem with cells randomly filling, ILP solver failed or the board
 * is not solvable - the function returns 0.
 *
 * If a legal board was generated successfully,
 * the game board is updated with y randomly choose cells from the solution.
 * otherwise, the game board is set back to an empty board.
 */
int attemptToGenerateBoard(Game * game, int x, int y, int * cellsArray, int * valuesArray){
	int i;
	int v;
	int r;
	int c;
	int val;
	int ILPSuccess;

	int N = game->N;
	int N2 = N*N;

	/* shuffle the cells indexes array */
	randomize(cellsArray, N2);
	/*
	 * for each of the first x cells in the shuffled cells indexes array:
	 * shuffle the valuesArray and for each possible value -
	 * try to put it in the current cell - if it's a legal placement ->
	 * break and go to the next cell. if there is no legal value for the cell -
	 * the function returns 0
	 */
	for (i = 0; i < x ; i++){
		/* get board cell indexes */
		r = cellsArray[i] / N;
		c = cellsArray[i] % N;
		/* shuffle the values array */
		randomize(valuesArray, N);
		for (v = 0; v < N ; v++){
			val = valuesArray[v];
			game->gameBoard[r][c] = val;
			if (checkAndMarkBoardErrorsInCol(game, r, c, 0) == 0 &&
					checkAndMarkBoardErrorsInRow(game, r, c, 0) == 0 &&
					checkAndMarkBoardErrorsInBlock(game, r, c, 0) == 0){
				break;
			}
			game->gameBoard[r][c] = 0;
		}
		/* if the cell doesn't have a legal value possibility - return 0 */
		if (game->gameBoard[r][c] == 0){
			return 0;
		}
	}

	/* here - each one of the x cells has a legal value */
	ILPSuccess = solveBoardWithILP(game);

	/* if ILp failed or the generated board isn't solvable - return 0 */

	if (ILPSuccess == 0){
		printGurobiError();
		return 0;
	}

	if (game->isSolvable == 0){
		return 0;
	}

	/* here - a legal and solvable board was generated */

	/* erase game board changes */
	for (i = 0; i < x ; i++){
		r = cellsArray[i] / N;
		c = cellsArray[i] % N;
		game->gameBoard[r][c] = 0;
	}

	/* randomly choose y cells not to be cleared */
	randomize(cellsArray, N2);
	for (i = 0; i < y; i++){
		r = cellsArray[i] / N;
		c = cellsArray[i] % N;
		game->gameBoard[r][c] = game->solvedBoard[r][c];
	}
	return 1;
}

/*
 * Add generate move to the game undo-redo moves list.
 * The function clears any move beyond the current move from the list,
 * then add the new move to the end of the list and mark it as the current move.
 *
 * The added move has a pointer to a list of InnerListNodes,
 * describing each one of the values changes of the Y non-empty cells in generate operation,
 * in order (up-down, left-right).
 */
void addGenerateMoveToMovesList (Game * game){
	int i;
	int j;
	InnerListNode * node;
	InnerListNode * firstNode = NULL;
	InnerListNode * prevNode = NULL;
	int N = game->N;

	/* clear any move (ListNode) beyond list->movePointer from the list */
	deleteAfterPointer(game->moves);

	/*
	 * go over the board up-down, left-right,
	 * insert each non-empty cell to the inner list,
	 * that describe the cell values changes.
	 */
	for ( i = 0; i < N ; i++){
		for (j = 0; j < N; j++){
			if (game->gameBoard[i][j] != 0){
				node = createInnerListNode(i,j,game->gameBoard[i][j],0);
				if(prevNode != NULL){
					prevNode->next = node;
				}
				else{
					firstNode = node;
				}
				prevNode = node;
			}
		}
	}
	insertAtTail(game->moves, firstNode);
}



/***** User commands *****/

void solve(Game * game, Command currCommand){

	int commandSuccess;

	/* change game mode to solve and save previous game mode */
	gameModeEnum prevGameMode = game->gameMode;
	game->gameMode = SOLVE_MODE;

	/* load puzzle from the supplied file */
	commandSuccess = getBoardFromFile(game, currCommand.filePath, currCommand.type);

	/* change game mode back to old mode if solve operation didn't succeed */
	if(commandSuccess != 1) {
		game->gameMode = prevGameMode;
		return;
	}

	 /* update according to the uploaded board:
	 * 1. number of non empty cell
	 * 2. isErroneous and mark cells with erroneous values (as negatives)
	 * 3. initalize new moves list
	 */
	checkAndMarkBoardErros(game);
	game->numOfNonZeroCells  = cntNumOfNonZeroCell(game);
	initMovesList(game);
	/* print the board after loading it from the file */
	printBoard(*game);
	
}

void edit(Game * game, Command currCommand){
	int commandSuccess;
	/* change game mode to solve and save previous game mode */
	gameModeEnum prevGameMode = game->gameMode;
	game->gameMode = EDIT_MODE;

	/* load puzzle from the supplied file, if such exists */
	if(currCommand.filePath != NULL){
		commandSuccess = getBoardFromFile(game, currCommand.filePath, currCommand.type);
		/* change game mode back to old mode if solve operation didn't succeed */
		if(commandSuccess != 1) {
			game->gameMode = prevGameMode;
			return;
		}
		/* update according to the uploaded board:
		 * 1. number of non empty cell
		 * 2. isErroneous and mark cells with erroneous values (as negatives)
		 */
		checkAndMarkBoardErros(game);
		game->numOfNonZeroCells  = cntNumOfNonZeroCell(game);
	}
	else {
		/* if there wasn't a file path in the user command -
		 * generate 9*9 empty board */
		genEmptyBoard(game);
	}
	/* initialize new moves list */
	initMovesList(game);
	/* print the board after loading it from the file */
	printBoard(*game);
}

void markErrors(Game * game, Command currCommand){
	/*
	 * if desired command value is 0 or 1 -
	 * the game markErrors value is changed
	 */
	if (currCommand.intArgs[0] == 0 || currCommand.intArgs[0] == 1){
		game->markErrors = currCommand.intArgs[0];
	}
	/*
	 * if desired command value is NOT 0 or 1 -
	 * an error message is printed and the command is not executed
	 */
	else {
		printInvalidMarkErrorsValError();
		return;
	}
}

void printBoard(Game game){
	int i;
	int j;

	for(i = 0 ;i < game.N ; i++){
		/* print separator row at the beginning of each Block */
		if(i % game.m == 0){
			SeparatorRow(game.N, game.m);
		}

		for(j = 0; j < game.N ; j++){
			/* print "|" at the beginning of each block row */
			if(j % game.n == 0){
				printf(PIPE);
			}
			/*
			 * Each cell is represented as 4 characters:
			 *		a. A space character.
			 *		b. Two digits for the cell value
			 *			1. in case of blank cell (game.gameBoard.arrayBoard[i][j] == 0 )
			 *				=> we use double space
			 *			2. else: we print the abs value of the cell (game.gameBoard.arrayBoard[i][j])
			 *				(reminder : negative value stands for erroneous values)
			 *		c.  1. A dot '.' for a fixed cell
			 *			2. an asterisk '*' for an erroneous cell when in Edit mode or the "Mark errors" parameter is set to 1,
			 *			3. space otherwise.
			 *			(note: A fixed cell is never marked as erroneous)
			 */

			/* a. space char */
			printf(SPACE);
			/* b. cell value */
			if (game.gameBoard[i][j] != 0){
				printf("%2d",abs(game.gameBoard[i][j]));
			}
			else {
				printf (DOUBLE_SPACE);
			}
			/*
			 * c.1 fixed cell (".")
			 */
			if (game.originalBoard[i][j] != 0){
				printf(FIXED_CELL_DOT);
			}
			else {
				/*
				 * c.2 erroneous cell when "Mark errors" == 1 or when in Edit mode ("*")
				 */

				if (game.gameBoard[i][j] < 0
					 && (game.markErrors == 1 || game.gameMode == EDIT_MODE)){
					printf(ERR_VAL_ASTERISK);
				}
				/*
				 * c.3 otherwise (" ")
				 */
				else {
					printf(SPACE);
				}
			}
		}

		/* end of row */
		printf (PIPE);
		printf(NEW_LINE);
	}

	/* end of board separator row */
	SeparatorRow(game.N, game.m);
}

void set(Game* game, Command currCommand) {

    int oldZ;

    /* x for the row, y for the column and z for the value to be set */
    int x = currCommand.intArgs[0];
    int y = currCommand.intArgs[1];
    int z = currCommand.intArgs[2];
    int N = game->N;

    /* check if x, y or z are not in their legal range:
     * 1 <= x,y <= N
     * 0 <= z <= N
     * if not - the command is not executed
     */
    if (!inputInRange(x,1,N) || !inputInRange(y,1,N)
    		|| !inputInRange(z,0,N) ) {
        printValueNotInRangeError(0, N);
        return;
    }

    /* if the cell is fixed - print message to user
	 * and the command is not executed
	 */
	if (game->originalBoard[y-1][x-1] != 0){
		printCellIsFixedError();
		return;
	}

	/* setCell Only sets the value of cell (x,y) to z*/
	oldZ = game->gameBoard[y-1][x-1];
	game->gameBoard[y-1][x-1] = z;

	/* add to moves list */
	addSetMoveToMovesList(game->moves, y-1, x-1, z, oldZ);

	/* update errors and number of non empty cells */
	updateErrorsAndEmptyCellNumAfterCellValueChange(game,z,oldZ);
	/* print the board */
	printBoard(*game);

	checkSolvedSuccessfullyAndFinish(game);

}

void validate(Game* game){
	/* print message to user if the board is erroneous
	 * and the command is not executed*/
	if (game->isErroneous == 1){
		printBoardContainErrors();
		return;
	}
	/* validates the board and update game->isSolvable field*/
	if(solveBoardWithILP(game) == 0) {
		printGurobiError();
		return;
	}
	/* print message to user according to isSolvable result after validation */
	if (game->isSolvable == 1){
		printValidationPassed();
	}
	else{
		printValidationFailed();
	}

}

void generate(Game* game, Command currCommand){
	int i;
	int * cellsNumberArr;
	int * cellsValuesArr;
	/* x and y are the values supplied by the user
	 * x - number of cells to randomly fill with legal values
	 * y -  number of cells to keep on board after solving the board */
	int x = currCommand.intArgs[0];
	int y = currCommand.intArgs[1];
	int N2 = game->N * game->N;
	/* boolean for board generation success */
	int generationSucceeded = 0;

	/* if either X or Y is invalid
	*  the program prints an error and the command is not executed
	*/
    if (!inputInRange(x,0,N2) || !inputInRange(y,0,N2)){
        printValueNotInRangeError(0, N2);
        return;
    }
	/*
	* This command is only available when the board is empty
	* if the board isn't empty, the program print message to user
	* and the command is not executed
	*/
	if (game->numOfNonZeroCells != 0){
		printBoardIsNotEmptyError();
		return;
	}
	if(y == 0) {
		return;
	}

	/* create an array in size NXN, where cell <i> value = i*/
	cellsNumberArr = (int *)malloc(N2 * sizeof(int));
	for (i = 0; i< N2; i++){
		cellsNumberArr[i] = i;
	}

	cellsValuesArr = (int *)malloc(game->N * sizeof(int));
	for (i = 0; i< game->N; i++){
		cellsValuesArr[i] = i+1;
	}

	/*
	 * Attempt to generate board at most MAX_GENERATES_ATTEMPTS times
	 * if one of the x randomly-chosen cells has no legal value available,
	 * or the resulting board has no solution -
	 * clear the board entirely and repeat the attempt to generate board
	 */
	for (i = 0; i<MAX_GENERATES_ATTEMPTS; i++){
		generationSucceeded = attemptToGenerateBoard(game, x, y,cellsNumberArr, cellsValuesArr);
		/* if the puzzle was generated successfully - print the board and update game attributes */
		if (generationSucceeded == 1){
			printBoard(*game);
			game->numOfNonZeroCells = y;
			game->isErroneous = 0;
			addGenerateMoveToMovesList(game);
			free(cellsNumberArr);
			free(cellsValuesArr);
			return;
		}
		clearBoards(game);
	}
	/* if we are here - board generation failed
	 * print message to user and the command is not executed*/
	free(cellsNumberArr);
	free(cellsValuesArr);
	printPuzzleGeneratorFailedError();
}

void undo(Game * game){
	/* checks if undo command is available and if so - execute undo */
	if(checkUndoAvailable(game->moves) == 1) {
		undoRedoMove(game, 1,1);
		setMovePointerBackward(game->moves);
	}
	/* if there are no moves to undo - print message to user
	 * and the command is not executed
	 */
	else{
		printNoMovesToUndoError();
		return;
	}
}

void redo(Game * game){
	/* checks if redo command is available and if so - execute redo */
	if(checkRedoAvailable(game->moves) == 1) {
		undoRedoMove(game, 0,1);
		setMovePointerForward(game->moves);
	}
	/* if there are no moves to redo - print message to user
	 * and the command is not executed
	 */
	else{
		printNoMovesToRedoError();
		return;
	}
}

void save(Game * game, Command currCommand){

	FILE * fp;
	int validSuccess;

	/* in Edit mode - if the board is erroneous -
	 * the command is not executed and a message is printed to the user
	 */
	if (game->gameMode == EDIT_MODE && game->isErroneous == 1){
		printBoardContainErrors();
		return ;
	}

	/* in Edit mode - we validate the board before saving it
	 * 1. if there is a problem with gurobi - print message to user
	 * 		and the command is not executed
	 * 2. if the board didn't pass validation (unsolvable) -
	 * 		print message to user and the command is not executed
	 * 	3. otherwise - proceed normally
	 */
	if (game->gameMode == EDIT_MODE){
		validSuccess = solveBoardWithILP(game);
		if (validSuccess == 0){
			printGurobiError();
			return;
		}
		if (game->isSolvable != 1){
			printBoardValidationError();
			return;
		}
	}

	/* open the file path to write board game to */
	fp = fopen(currCommand.filePath, "w");
	if (fp == NULL){
		printFileCreationError();
		/* command isn't executed */
		return;
	}
	/* if no errors occur then the game board is saved to the file
	 * in the required file format
	 * and a message is printed to the user */
	if (saveBoardToFile (*game, fp) == 1 ){
		printSavedToFile(currCommand.filePath);
	}

	if (fclose(fp) != 0){
		printFileCloseError();
		exit(0);
	}
}

void hint(Game * game, Command currCommand){

	/* x and y are the values supplied by the user
	 * x - cols
	 * y - rows */
	int x = currCommand.intArgs[0];
	int y = currCommand.intArgs[1];

	/*
	 * if X or Y are invalid (1-N according to the current board)
	 * print error and the command is not executed
	 */
	if (inputInRange(x, 1, game->N) == 0 ||
		inputInRange(y, 1, game->N) == 0) {
		printValueNotInRangeError(1, game->N);
		return;
	}

	/* if the board is erroneous - print message to user
	 * and the command is not executed*/
	if (game->isErroneous == 1){
		printBoardContainErrors();
		return;
	}
	/* if the cell is fixed - print message to user
	 * and the command is not executed
	 */
	if (game->originalBoard[y-1][x-1] != 0){
		printCellIsFixedError();
		return;
	}
	/* if the cell isn't fixed but already has value -
	 *  print message to user and the command is not executed
	 */
	if (game->gameBoard[y-1][x-1] != 0){
		printCellAlreadyContainValueError();
		return;
	}

	/* validates the board and update game->isSolvable field*/
	if(solveBoardWithILP(game) == 0) {
		printGurobiError();
		return;
	}
	/* print message to user according to isSolvable result after validation
	 * if the board is solvable - prints the hint value to cell <x,y> */
	if (game->isSolvable == 1){
		printHint(game->solvedBoard[y-1][x-1]);
	}
	else{
		printBoardIsUnsolvable();
	}

}

void numSolutions(Game * game){
	int i;
	int j;
	int numSolutions;

	/* if the board is erroneous - print message to user
	 * and the command is not executed*/
	if (game->isErroneous == 1){
		printBoardContainErrors();
		return;
	}

	/* copy game board to solved board */
	for (i = 0; i< game->N; i++){
		for(j = 0; j< game->N; j++){
			game->solvedBoard[i][j] = game->gameBoard[i][j];
		}
	}

	/* get board num of solutions - if the board is already filled - return 1 instead of running BT algo */
	if(game->numOfNonZeroCells == game->N * game->N) {
		numSolutions = 1;
	}
	else {
		numSolutions = cntNumSolutions(game);
	}
	/* print the board num of solutions */
	printNumOfBoardSolutions(numSolutions);

	/* copy solved board to game board */
	for (i = 0; i< game->N; i++){
		for(j = 0; j< game->N; j++){
			game->gameBoard[i][j] = game->solvedBoard[i][j];
		}
	}

	/* print an additional message according to board num of solutions */
	if (numSolutions == 1){
		printOnlyOneSolution();
	}
	else if (numSolutions > 1) {
		printMoreThanOneSolution();
	}
}

void autofill(Game * game){
	int i;
	int j;
	int onlyLegalValueForCell;
	InnerListNode * firstNode = NULL;
	InnerListNode * currNode = NULL;
	InnerListNode * prevNode = NULL;

	/* if the board is erroneous - print message to user
	 * and the command is not executed*/
	if (game->isErroneous == 1){
		printBoardContainErrors();
		return;
	}

	for ( i = 0; i < game->N; i++){
		for( j = 0; j < game->N; j++){
			/* if the cell is not  empty */
			if(game->gameBoard[i][j] != 0){
				continue;
			}
			onlyLegalValueForCell = getCellObiviousValue(game,i,j);
			if (onlyLegalValueForCell != 0){
				currNode = createInnerListNode(i,j,onlyLegalValueForCell,0);
				if(prevNode != NULL){
					prevNode->next = currNode;
				}
				else{
					firstNode = currNode;
				}
				prevNode = currNode;
			}
		}
	}

	/* if autofill didn't change the board */
	if (firstNode == NULL){
		printBoard(*game);
		return;
	}

	currNode = firstNode;
	while (currNode != NULL){
		i = currNode->cellChange[0];
		j = currNode->cellChange[1];
		onlyLegalValueForCell = currNode->cellChange[2];
		/* update cell <i,j> value */
		game->gameBoard[i][j] = onlyLegalValueForCell;
		game->numOfNonZeroCells++;
		printCellSetTo(i,j,onlyLegalValueForCell);
		currNode = currNode->next;
	}
    checkAndMarkBoardErros(game);
	printBoard(*game);

	/* clear any move (ListNode) beyond list->movePointer from the list */
	deleteAfterPointer(game->moves);
	insertAtTail(game->moves, firstNode);

	checkSolvedSuccessfullyAndFinish(game);
}

void reset(Game* game){
	/* while undo command is available execute undo */
	while(checkUndoAvailable(game->moves) == 1) {
		undoRedoMove(game, 1,0);
		setMovePointerBackward(game->moves);
	}
	/* after reverting all the moves - clear moves list
	 * (keep only sentinels)
	 * and print message to user
	 */
	if(game->moves->head != NULL) {
		freeLinkedListFromStartPoint(game->moves->head->next);
	}
	printResetMessage();
}

void exitGame(Game * game){
	if (game->gameMode != INIT_MODE){
		freeGameBoards(game);
	}
	freeLinkedList(game->moves);
	printExiting();
	game->gameOver = 1;
}
