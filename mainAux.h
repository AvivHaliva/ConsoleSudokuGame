/************************************
* FILES NAMES :        MainAux.c , MainAux.h
*
* DESCRIPTION :
*       Auxiliary functions for the entire sudoku game project - print user messages, print error messgaes,
*       get line from input and randomize an array. File main goal is to allow easy dynamic changes in messages printing.
*
* PUBLIC FUNCTIONS :
*       void SeparatorRow(int N , int m)
*       void printStartSudokuString()
*       void printEnterCommand()
*       void printInvalidCommandError()
*       void printInvalidMarkErrorsValError()
*       void printFgetsError()
*       void printSolveFileAccessError()
*       void printEditFileAccessError()
*       void printFileCloseError()
*       void printFileFormatError()
*       void printMemoryAllocationError()
*       void printBoardValidationError()
*       void printFileCreationError()
*       void printValueNotInRangeError(int rangeStart, int rangeEnd)
*       void printCellIsFixedError()
*       void printPuzzleSolvedSuccessfullyMessage()
*       void printPuzzleSolutionErroneous()
*       void printSavedToFile(char * filePath)
*       void printBoardContainErrors()
*       void printValidationPassed()
*       void printValidationFailed()
*       void printCellAlreadyContainValueError()
*       void printBoardIsUnsolvable()
*       void printHint(int Z)
*       void printBoardIsNotEmptyError()
*       void printPuzzleGeneratorFailedError()
*       void printExiting()
*       void printGurobiError()
*       void printNoMovesToUndoError()
*       void printNoMovesToRedoError()
*       void printUndoRedoMove(int x, int y, int z, int oldZ, int isUndo)
*       void printResetMessage()
*       void printCellSetTo(int i, int j, int val)
*       void printNumOfBoardSolutions(int numSolutions)
*       void printOnlyOneSolution()
*       void printMoreThanOneSolution()
*
*       void randomize ( int * arr, int n )
*       char *getLine(FILE * f)
*
************************************/

/* DELIMITER:
 * board files and command lines can contain any type of space between args */
#define DELIMITER " \t\r\n"

/* printing constants */
#define PIPE "|"
#define NEW_LINE "\n"
#define DASH "-"
#define SPACE " "
#define DOUBLE_SPACE "  "
#define FIXED_CELL_DOT "."
#define ERR_VAL_ASTERISK "*"
#define EMPTY_CELL "_"


/* prints a series of 4N+m+1 dashes as the separator row */
void SeparatorRow(int N , int m);

/*
 * print opening game String
 */
void printStartSudokuString();

/*
 * print enter command
 */
void printEnterCommand();

/*
 * print the invalid command error
 */
void printInvalidCommandError();

/*
 * print message to user in case of value different than 0 or 1 to the mark Errors command
 */
void printInvalidMarkErrorsValError();

/*
 * print message to user in case of fgets error
 */
void printFgetsError();

/*
 * print message to user in case of file access problems in solve mode
 */
void printSolveFileAccessError();

/*
 * print message to user in case of file access problems in edit mode
 */
void printEditFileAccessError();


/*
 * print message to user in case of file closing problem
 */
void printFileCloseError();

/*
 * print message to user in case of file format problems
 */
void printFileFormatError();

/*
 * print message to user in case of problem with memory allocation
 */
void printMemoryAllocationError();

/*
 * print message to user in case board validation failed
 * in save operation (in edit mode)
 */
void printBoardValidationError();

/*
 * print message to user in case there was a problem with file creation
 * or modification
 */
 void printFileCreationError();


 /*
  *  Prints message to user in case of input that is our of board range
  */
 void printValueNotInRangeError(int rangeStart, int rangeEnd);

 /*
  * Prints a message to the user in case the cell that he's trying to set is fixed
  */
 void printCellIsFixedError();

 /*
  *Prints a message to user when the puzzle is solved successfully
  */
 void printPuzzleSolvedSuccessfullyMessage();

 /*
  * Prints a message to user when all the board is full but the solution is invalid
  */
 void printPuzzleSolutionErroneous();

 /*
  * print message to the user when saving board to a file succeeded:
  * "Saved to: filePath\n", where filePath is the exact filename provided by the user
  */
void printSavedToFile(char * filePath);

/*
 * print message to the user if the board is erroneous
 */
void printBoardContainErrors();

/*
 * print message to user if the validation passed and the board is solvable
 */
void printValidationPassed();

/*
 * print message to user if the validation failed and the board is not solvable
 */
void printValidationFailed();

/*
 * print message to user if the desired cell already contain a value
 */
void printCellAlreadyContainValueError();

/*
 * print message to user if the board is unsolvable
 */
void printBoardIsUnsolvable();

/*
 * print Hint to user - set cell value to Z
 */
void printHint(int Z);

/*
 * print message to user if the board is not empty
 */
void printBoardIsNotEmptyError();

/*
 * print message to user if puzzle generator failed
 */
void printPuzzleGeneratorFailedError();

/*
 * print Exiting message to user in case of exit command
 */
void printExiting();

/*
 * print message to user in case of error in gurobi ILP execution
 */
void printGurobiError();

/*
 * print message to user in case there are no moves to undo
 */
void printNoMovesToUndoError();

/*
 * print message to user in case there are no moves to redo
 */
void printNoMovesToRedoError();

/*
 * print message to user with the redo \ undo operation details
 * empty cells are printed as _
 */
void printUndoRedoMove(int x, int y, int z, int oldZ, int isUndo);


/*
 * print message to user after reset execution
 */
void printResetMessage();

/*
 * print the cell value change : cell <i,j> value changed to val
 */
void printCellSetTo(int i, int j, int val);

/*
 * print the number of board solutions
 */
void printNumOfBoardSolutions(int numSolutions);

/*
 * print a suitable message if the board has only one solution
 */
void printOnlyOneSolution();

/*
 * print a suitable message if the board has more than one solution
 */
void printMoreThanOneSolution();

/*
 * generate a random permutation of arr[]
 * n is the length of the arr
 * using  srand ( time(NULL) ) for ramdomization
 */
void randomize ( int * arr, int n );

/*
 * read line of unknown length from a file
 * implementation of the function getline from GNU module
 */
char *getLine(FILE * f);

