
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mainAux.h"

void SeparatorRow(int N , int m){
	int k;
	for (k = 0; k < 4*N+m+1 ; k++){
		printf(DASH);
	}
	printf(NEW_LINE);
}

void printStartSudokuString(){
	printf("Sudoku\n------\n");
}

void printEnterCommand(){
	printf("Enter your command:\n");
}

void printInvalidCommandError(){
	printf("ERROR: invalid command\n");
}

void printInvalidMarkErrorsValError(){
	printf("Error: the value should be 0 or 1\n");
}

void printFgetsError(){
	printf("Error: Fgets Error\n");
}

void printSolveFileAccessError(){
	printf("Error: File doesn't exist or cannot be opened\n");
}

void printEditFileAccessError(){
	printf("Error: File cannot be opened\n");
}

void printFileCloseError(){
	printf("Error: File cannot be closed\n");
}

void printFileFormatError(){
	printf("Error: File format is incorrect\n");
}

void printMemoryAllocationError(){
	printf( "Error: Memory Allocation Failed\n");
 }

void printBoardValidationError(){
	printf("Error: board validation failed\n");
}

 void printFileCreationError(){
	 printf("Error: File cannot be created or modified\n");
 }

 void printValueNotInRangeError(int rangeStart, int rangeEnd){
     printf("Error: value not in range %d-%d\n",rangeStart, rangeEnd);
 }

 void printCellIsFixedError(){
     printf("Error: cell is fixed\n");
 }

 void printPuzzleSolvedSuccessfullyMessage(){
     printf("Puzzle solved successfully\n");
 }

 void printPuzzleSolutionErroneous(){
     printf("Puzzle solution erroneous\n");
 }

void printSavedToFile(char * filePath){
	printf( "Saved to: %s\n", filePath);
}

void printBoardContainErrors(){
	printf("Error: board contains erroneous values\n");
}
void printValidationPassed(){
	printf("Validation passed: board is solvable\n");
}

void printValidationFailed(){
	printf("Validation failed: board is unsolvable\n");

}
void printCellAlreadyContainValueError(){
	printf("Error: cell already contains a value\n");
}

void printBoardIsUnsolvable(){
	printf("Error: board is unsolvable\n");
}

void printHint(int Z){
	printf("Hint: set cell to %d\n", Z);
}

void printBoardIsNotEmptyError(){
	printf("Error: board is not empty\n");
}

void printPuzzleGeneratorFailedError(){
	printf("Error: puzzle generator failed\n");
}

void printExiting(){
	printf("Exiting...\n");
}

void printGurobiError(){
	printf("Gurobi optimization Failed\n");
}

void printUndoRedoMove(int x, int y, int z, int oldZ, int isUndo){
	char * command;
	int tmp;

	if(isUndo == 1){
		command = "Undo";
	}
	else{
		command = "Redo";
		tmp = z;
		z = oldZ;
		oldZ = tmp;
	}

	if(z == 0 && oldZ == 0) {
		printf("%s %d,%d: from %s to %s\n", command, y+1, x+1,EMPTY_CELL, EMPTY_CELL);
	}
	else if(z == 0) {
		printf("%s %d,%d: from %s to %d\n", command, y+1, x+1,EMPTY_CELL, abs(oldZ));
	}
	else if(oldZ == 0) {
		printf("%s %d,%d: from %d to %s\n", command, y+1, x+1,abs(z), EMPTY_CELL);
	}
	else {
		printf("%s %d,%d: from %d to %d\n", command, y+1, x+1,abs(z), abs(oldZ));
	}
}

void printNoMovesToUndoError(){
	printf("Error: no moves to undo\n");
}

void printNoMovesToRedoError(){
	printf("Error: no moves to redo\n");
}

void printResetMessage(){
	printf("Board reset\n");
}

void printCellSetTo(int i, int j, int val){
	printf("Cell <%d,%d> set to %d\n", j+1, i+1, val);
}

void printNumOfBoardSolutions(int numSolutions){
	printf("Number of solutions: %d\n", numSolutions);
}

void printOnlyOneSolution(){
	printf("This is a good board!\n");
}

void printMoreThanOneSolution(){
	printf("The puzzle has more than 1 solution, try to edit it further\n");
}


/* A utility function to swap to integers */
void swap (int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void randomize (int * arr, int n )
{
	int i;
	int j;
    srand ( time(NULL) );

    /* Start from the last element and swap one by one. We don't
    need to run for the first element so i > 0 */
    for (i = n-1; i > 0; i--)
    {
        /* Pick a random index from 0 to i */
        j = rand() % (i+1);

        /* Swap arr[i] with the element at random index */
        swap(&arr[i], &arr[j]);
    }
}

/*
 * gets a pointer to a File
 * and read line on unknown length from the file
 * if the file has read EOF return NULL
 */
char * getLine(FILE * f) {
    int size = 0;
    int len  = 0;
    int last = 0;
    char *buf = NULL;

    do {
    	/* BUFSIZ is a constant of stdio.h */
        size += BUFSIZ;
        buf = realloc(buf, size);
        /* if memory allocation error occurred - exit */
        if (buf == NULL) {
        	printMemoryAllocationError();
        	exit(0);
        }
        /* if fgets error  - exit*/
        if (fgets(buf + last, size, f) == NULL){
        	if(feof(f)) {
        		free(buf);
				return NULL;
			}
        	printFgetsError();
        	exit(0);
        }


        len = strlen(buf);
        last = len - 1;

    } while (!feof(f) && buf[last] != '\n');
    return buf;
}
