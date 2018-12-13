/***********************************************************************
* FILES NAMES :       Game.c , Game.h
*
* DESCRIPTION :
*         Define game entities and maintain the sudoku game logic - supporting sudoku game commands and additional operations
*
* PUBLIC FUNCTIONS :
*         void initMovesList(Game * game)
*         void initalizeGameBoards(Game * game, int m, int n)
*         void freeGameBoards (Game * game)
*         void initalizeGameAtINIT(Game* game)
*
*         void solve(Game * game, Command currCommand)
*         void edit(Game * game, Command currCommand)
*         void markErrors(Game * game, Command currCommand)
*         void printBoard(Game game)
*         void set(Game * game, Command currCommand)
*         void validate(Game* game)
*         void generate(Game* game, Command currCommand)
*         void undo(Game * game)
*         void redo(Game * game)
*         void save(Game * game, Command currCommand)
*         void hint(Game * game, Command currCommand)
*         void numSolutions(Game * game)
*         void autofill(Game * game)
*         void reset(Game * game)
*         void exitGame(Game * game)
*
*         int checkAndMarkBoardErrorsInRow(Game * game, int i, int j, int mark)
*         int checkAndMarkBoardErrorsInCol(Game * game, int i, int j, int mark)
*         int checkAndMarkBoardErrorsInBlock(Game * game, int i, int j , int mark)
*
* NOTES :
*       1. Game.h contains the definition of the following:
*           a. gameModeEnum - an enum describing the current game mode according to user operations. game always starts in INIT mode.
*           b. Game - a struct representing the desired fields of a current sudoku game:
*                     boards, current mode, undo-redo list, current mark Errors value etc.
*                     more details can be found in the Game struct definition documentation below.
*       2.  The public function in Game.h support the user legal commands execution, for example -
*       	Editing and creating new sudoku puzzles of any size, Saving and loading puzzles from files,
*       	Auto-filling option for obvious cells (where only a single value is valid).
*       	The rest of the public functions are used to initialize the game entities
*           and also check and mark board errors, if such exists.
*       3.  The functions in Game.c support those operations.
*           A suitable documentation of each non-public function can be found above its implementation in game.c file.
*
***********************************************************************/

#include "LinkedList.h"

/* Default board size constants */
#define DEFAULT_BOARD_n 3
#define DEFAULT_BOARD_m 3

/* max ILP generation attempts constant */
#define MAX_GENERATES_ATTEMPTS 1000

/*
 * gameModeEnum represent the current gameMode of the user:
 * INIT_MODE, SOLVE_MODE, EDIT_MODE
 */
typedef enum gameModeEnum {
	INIT_MODE, SOLVE_MODE, EDIT_MODE} gameModeEnum;

/*
 * Game struct represent all the following elements for the current game:
 *
 * 1. Boards:
 * 		a. gameBoard matrix - the board that the user change with commands
 * 			i.   erroneous cells are represented as NEGATIVE values
 * 			ii.  empty cells are represented as 0
 * 			iii. other cells - positive int values
 * 		b. originalBoard matrix - board that has ONLY FIXED CELLS values,
 * 			others cells are 0
 * 		c. solvedBoard matrix - board that is filled with the latest legal solution that was
 * 			found for this game gameBoard
 * 		d. dimensions : N (boards rows/cols size), n (block cols size), m (bloc rows size)
 *
 * 2. isSolvable: 1 iff the gameBoard has a valid solution
 *
 * 3. markErrors : determines whether errors in the Sudoku puzzle are displayed
 * 		a. 1 for marking errors, 0 otherwise
 * 		b. The default value is 1.
 * 		c. In Edit mode we always mark errors, regardless of the value of this param.
 *
 * 4. gameOver: 1 iff the game is over
 * 				(user finished solving the board or chose the exit command)
 *
 * 5. gameMode:
 * 		a. enum representing the current game mode: solve, edit or init
 * 		b. by creating game instance - gameMode is initialize to INIT_MODE
 * 		c. entering solve or edit mode is via Solve or Edit command accordingly
 *
 * 6. moves:
 * 		a. a pointer to a doubly-linked list of the user's moves that maintains
 * 			 	a pointer to the current move.
 * 		b. the user may freely traverse this list (move current move pointer)
 * 		 		by using the redo / undo commands
 *
 * 7. numOfNonZeroCells: count the number of non empty cells in game board
 *
 * 8. isErroneous: boolean equals 1 iff the board contains error
 *
 */
typedef struct Game{

    int ** gameBoard;
    int ** originalBoard;
    int ** solvedBoard;

    /* boards dimension: N*N */
    int N;

    /* block dimensions: m rows, n cols */
    int n;
    int m;

    /*
     * 1 if board is solvable and 0 if not
     * */
    int isSolvable;

    /*
     * 1 for marking errors and 0 for not marking them
     * */
    int markErrors;

    /*
     * 1 if game is over and 0 if it is still going on
     * */
    int gameOver;

    /*
     * gameMode enum represents the current mode the of the user :
     * INIT_MODE, SOLVE_MODE or EDIT_MODE
     * */
    gameModeEnum gameMode;


    /*
     * A link list that holds all the operations for undo and redo
     * */
    LinkedList * moves;

    int numOfNonZeroCells;

    int isErroneous;

}Game;



/***** Initialization & Memory operation*****/

/*
 * initialize moves linked list
 * with memory allocation and setting pointers to NULL
 */
void initMovesList(Game * game);

/*
 * gets game, m and n
 * initialize boards dimension and allocate memory for boards array
 */
void initalizeGameBoards(Game * game, int m, int n);

/*
 * free all the game boards allocated memory
 */
void freeGameBoards (Game * game);

/*
 * Initialize the game with the start values:
 * 	1. game starts in INIT MODE
 * 	2. game starts with mark errors default value - 1
 * 	3. game start with gameOver value set to 0
 * 	4. all game boards are initialized to NULL
 * 	NOTE: the value of mark_errors is changed to the default value
 * 	iff initializeMarkErrors == 1
 */
void initalizeGameAtINIT(Game* game, int initializeMarkErrors);



/***** Errors: check & mark board accordingly *****/

/*
 * gets a pointer to a game entity,
 * indexes i (for row) and j (for col)
 * and mark flag - 1 iff we want to mark the erroneous values.
 *
 * the function goes over all the cells in the i-th row and check
 * if there is a column k != j,
 * so that cell <i,k> value is equal to cell <i,j> value.
 * if so, the function returns 1 (return 1 iff there are erroneous values
 * in the i-th row).
 *
 * if the mark flag is equal to 1 and erroneous values, as described, were found -
 * the function also set cells <i,k> and <i,j> value to negative.
 */
int checkAndMarkBoardErrorsInRow(Game * game, int i, int j, int mark);

/*
 * gets a pointer to a game entity,
 * indexes i (for row) and j (for col)
 * and mark flag - 1 iff we want to mark the erroneous values.
 *
 * the function goes over all the cells in the j-th col and check
 * if there is a row k != j,
 * so that cell <k,j> value is equal to cell <i,j> value.
 * if so, the function returns 1 (return 1 iff there are erroneous values
 * in the j-th col).
 *
 * if the mark flag is equal to 1 and erroneous values, as described, were found -
 * the function also set cells <k,j> and <i,j> value to negative.
 */
int checkAndMarkBoardErrorsInCol(Game * game, int i, int j, int mark);

/*
 * gets a pointer to a game entity,
 * indexes i (for row) and j (for col)
 * and mark flag - 1 iff we want to mark the erroneous values.
 *
 * the function goes over all the cells in <i,j> suitable block and check
 * if there is a different cell in the same block, <r,c>,
 * so that cell <r,c> value is equal to cell <i,j> value.
 * if so, the function returns 1 (return 1 iff there are erroneous values
 * in <i,j> block).
 *
 * if the mark flag is equal to 1 and erroneous values, as described, were found -
 * the function also set cells <r,c> and <i,j> value to negative.
 */
int checkAndMarkBoardErrorsInBlock(Game * game, int i, int j , int mark);



/***** User commands  *****/

/*
 * 	a. Starts a puzzle in Solve mode
 * 		i.   puzzle is loaded from a file with the name "X",
 * 			 where X includes a full or relative path to the file.
 * 			 we get the path, X, if supplied, from the parsed command - currCommand.filePath
 * 		ii.  gameMode is set to SOLVE_MODE
 * 		iii. update according to the uploaded board:
 * 				1. number of non empty cells
 * 				2. isErroneous value and mark cells with erroneous values as negatives
 * 		iv.  initialize a new "moves" LinkedList
 *
 * 	b. if the file doesn't exist or can't be opened -
 * 		i.  an error is printed and the command is not executed.
 *	c. print the board
 * 	d. NOTE:
 * 		i. 	we assume that the file contains valid data and is correctly formatted.
 * 		ii. Any unsaved work is lost.
 */
void solve(Game * game, Command currCommand);

/*
 *	a. Starts a puzzle in Edit mode, loaded from a file with the name "X",
 *		where X includes a full or relative path to the file.
 *			i.  we get the path,X, if supplied, from the parsed command - currCommand.filePath
 *			ii. In case the file does not exist, cannot be created,
 *				or cannot be opened -
 * 	   			the program prints error and the command is not executed.
 *	b. The parameter X is optional. If no parameter is supplied,
 *		the program enter Edit mode with an empty board.
 *	c. in both cases:
 *		i.   gameMode is set to EDIT_MODE
 * 		ii.  update according to the uploaded board:
 * 				1. number of non empty cells
 * 				2. isErroneous value and mark cells with erroneous values as negatives
 * 		iii. initialize a new "moves" LinkedList
 * 	d. print the board
 *
 *	e. NOTE:
 *		i.   We assume the file, if supplied, contains valid data and is correctly formatted.
 *		ii.  in Edit mode, the value of the "mark errors" parameter is always considered to be 1,
 *			 ignoring its actual value.
 *		iii. Any unsaved work is lost.
 */
void edit(Game * game, Command currCommand);

/*
 *  gets the game and the parsed command:
 *	a. Sets the game->markError value to X, where X is either 0 or 1.
 *	b. If X is not 0 or 1, the program prints error and the command is not executed.
 */
void markErrors(Game * game, Command currCommand);

/*
 * The board will be printed in the following format:
 *	1. Separator row
 *	2. Repeat for each row of blocks:
 *		a. Cell row
 *			i. Repeat for each of the row of cells
 *		b. Separator row
 *
* Cell row – a pipe character '|' starts and ends the row,
* 			 and separates each block.
* Each cell is represented as 4 characters:
* 	a. A space character.
* 	b. Two digits for the cell value (printf format "%2d"). blank cell  will have 2 spaces instead of a value.
* 	c. A dot for a fixed cell, an asterisk for an erroneous cell when in Edit mode or the "Mark errors" parameter is set to 1,
*  	   or space otherwise. Note that a fixed cell is never marked as erroneous
*/
void printBoard(Game game);

/*
 * Sets the value of cell <X,Y> to Z.
 *
 * 1. If X, Y, or Z are invalid, the program prints an error and the command is not executed.
 * 2. If cell <X,Y> is fixed the program prints an error and the command is not executed.
 * 3. OtherWise:
 * 		a. The user may empty a cell by setting Z=0.
 * 		b. Clear any move beyond the current move from the undo/redo list,
 * 			then add the new move to the end of the list and mark it as the current move.
 * 		c. An erroneous input is allowed but an error is issued
 * 			if the "mark errors" param is set to 1 or we're in Edit mode.
 * 		d. After updating cell value:
 * 			i.  game board is printed.
 * 			ii. update board errors and number of non empty cells
 * 		e. NOTE: if we are in Solve mode:
 *			i.  after the game board is printed, if this is the last cell to be filled
 *			 	(NumOfNonEmptyCell = 0) then the board is immediately validated.
 *			ii. If the validation fails - we remain in Solve mode and the program print that the solution isn't right.
 *			 	If the validation passes - the program prints that the puzzle is solved
 *			 	and the game mode is set to Init.
 * NOTE: the function gets the user supplied X,Y,Z values from currCommand
*/
void set(Game * game, Command currCommand);

/*
 *	Validates the current board using ILP with solveBoardWithILP function,
 *	ensuring it is solvable.
 *	1. If the board is erroneous, the program prints error and the command is not executed.
 *	2. If the board is found to be solvable, the program prints suitable message to user.
 *  	Otherwise, the program prints that the board is unsolvable.
 */
void validate(Game* game);

/*
 * Generates a puzzle by randomly filling X cells with random legal values,
 * running ILP to solve the resulting board, and then clearing all but Y random cells.
 *
 * 1. If either X or Y is invalid (not a number, or more than the number of empty cells in the current board), the program prints an error and the command is not executed.
 * 2. Generate command is only available when the board is empty, so if the board isn't empty the program prints an error and the command is not executed.
 * 3. Otherwise:
 *     a. Randomly choose X cells, filling each with a legal random value.
 *     b. Once X cells contain values, run ILP to solve the resulting board.
 *     c. After the board is solved - randomly choose Y cells, and clear the values of all other cells.
 *     d. i.   If one of the X randomly-chosen cells has no legal value available, or the resulting board has no solution (the ILP solver fails),
 *                 the board is entirely clear and we repeat steps a-c.
 *                 After 1000 such iterations, the program prints an error and the command is not executed.
 *        ii.  If the puzzle was generated successfully:
 *             - print the board
 *             - update isErroneous to 0 (false) and numOfNonZeroCells to Y
 *             - clear any move beyond the current move from the undo/redo list,
 *                 then add the new generate move to the end of the list and mark it as the current move.
 *
 * NOTE: the function gets the user supplied X and Y values from currCommand
 */
void generate(Game* game, Command currCommand);

/*
 * Undo previous moves done by the user.
 *
 * if there was a move to undo, the program prints the board and then prints
 * the changed cell indexes, value before undo and value after undo.
 * 		1. set the current move pointer to the previous move
 * 		2. update the board accordingly
 * 	  		i.   update the changed cell(s) value(s)
 * 	  		ii.  check errors and update board errors
 * 	  		iii. update number of non empty cells in the board
 * 		3. Note: This does not add or remove any item to/from the list.
 *
 * if there are no moves to undo, the program prints error and the command is not executed.
 */
void undo(Game * game);

/*
 * Redo a move previously undone by the user.
 *
 * If there was a move to redo, the program prints the board and then prints
 * the changed cell indexes, value before redo and value after redo.
 * 		1. set the current move pointer to the next move
 * 		2. update the board accordingly
 * 			i.   update the changed cell(s) value(s)
 * 	  		ii.  check errors and update board errors
 * 	  		iii. update number of non empty cells in the board
 * 		3.This does not add or remove any item from the list.
 * if there are no moves to redo the program prints error and the command is not executed.
 */
void redo(Game * game);

/* Saves the current game board to the specified file, where X includes a full or relative path to the file.
 *
 * a. In Edit mode, if the board is erroneous the program prints ERROR and the command is not executed.
 * b. In Edit mode, the board is validated before saving. If the validation passes, then proceed normally (. If the validation fails, the
 * 	  program prints error and the command is not executed.
 * c. If the file cannot be created or modified, the program prints error and the command is not executed.
 * d. If no errors occur then the game board is saved to the file and the program prints "Saved to: X\n", where X is the exact filename provided by the user.
 *    The format of the file is as follows (a simple text file):
 *         i.   The first line contains the block size m n.  These are two integers, separated by a single space.
 *         ii.  Each line contains a single row of the board.
 *         iii. Each cell contains the cell's value separated with single spaces. The value in empty cells is 0.
 *              if the cell is "fixed", follow its value with a dot '.'.
 * e. NOTE:
 *     1. Saving the puzzle to a file does not modify the Redo/Undo list in any way, i.e., a reset command (described below) will still revert to the state of the originally loaded file.
 *     2. In Edit mode, all cells containing values are marked as "fixed".
 */
void save(Game * game, Command currCommand);

/*
 * Give a hint to the user by showing the solution of a single cell X,Y.
 *
 * In the following cases the program prints suitable error and the command is not executed:
 * 		1. X or Y are invalid (1-N according to the current board)
 * 		2. The board is erroneous
 * 		3. cell <X,Y> is fixed
 * 		4. cell <X,Y> already contains a value
 *
 * Otherwise, Run ILP to solve the board using solveBoardWithILP function.
 *      1. if the board is unsolvable -
 *         the program prints Error and the command is not executed.
 *		2. Otherwise, the program prints: "Hint: set cell to Z\n"
 *		   where Z is the value of cell <X,Y> found by solveBoardWithILP.
 */
void hint(Game * game, Command currCommand);

/*
 *  Print the number of solutions for the current board.
 *  
 *  a. If the board is erroneous the program prints an error and the command is not executed.
 *  b. otherwise:
 *      1. Run an exhaustive backtracking for the current board.
 *         The exhaustive backtracking algorithm exhausts all options for the current board. 
 *         Once done, the program prints the number of solutions for the current board returned by the exhaustive backtracking algorithm.
 *      2. If the board has only a single solution, the program additionally prints a suitable message.
 *         Otherwise, the program prints that the puzzle has multiple solutions.
 */
void numSolutions(Game * game);

/*
 * Automatically fill "obvious" values – cells which contain a single legal value.
 *
 * 1. If the board is erroneous the program prints an error and the command is not executed.
 * 2. Otherwise:
 *     i.   function goes over the board and check the legal values of each empty cell.
 *     ii.  if a cell <X,Y> has a single legal value, fill it with the value
 *     		and the program prints the cell indexes and new value.
 *     iii. once the command is finished:
 *             - print the board
 *             - Clear any move beyond the current move from the undo/redo list,
 *               then add the new autofill move to the end of the list and mark it as the current move.
 *             - if after the command execution, the board is entirely filled (NumOfNonEmptyCell = 0)->
 *             	 the program prints that the puzzle is solved
 *  			 and the game mode is set to Init.
 *
 * 3. NOTE:
 *     i.   the function go over the cells from left-to-right, then top-to-bottom.
 *     ii.  only cells that had a single legal value BEFORE the command was executed should be filled,
 *     iii. after performing this command, additional cells may contain obvious values.
 *          however, this command performs ONLY A SINGLE iteration and these cells won't be filled.
 */
void autofill(Game * game);

/*
 * Undo all moves, reverting the board to its original loaded state.
 *
 * The command goes over the entire undo/redo list and reverts all moves (no output is provided).
 * Once the board is reset, the undo/redo list is cleared entirely, and the program prints a suitable message.
*/
void reset(Game * game);

/*
 * Terminates the program by setting game->gameOver = 1.
 * free all allocated memory resources, if exists,
 * and print Exiting message to the user
 */
void exitGame(Game * game);
