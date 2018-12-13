#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gurobi_c.h"
#include "Game.h"
#include "mainAux.h"

/*
 * allocate memory for Gurobi model and environment
 * if there is a problem with one of the memory allocations
 * the program prints error to the user and exit
 */
void allocateGurobi (int N, double ** lb, char ** vtype, char *** names,
		char ** namestorage,  int ** ind, double ** val){

	int N3 = N*N*N;

	*lb = (double *) malloc(N3*sizeof(double));
	if (*lb == NULL){
		printMemoryAllocationError();
		exit(0);
	}

	*vtype = (char *) malloc(N3*sizeof(char));
	if (*vtype == NULL){
		printMemoryAllocationError();
		exit(0);
	}

	*names = (char**) malloc(N3 * sizeof(char*));
	if (*names == NULL){
		printMemoryAllocationError();
		exit(0);
	}

	*namestorage = (char*) malloc(10*N3);
	if (*namestorage == NULL){
		printMemoryAllocationError();
		exit(0);
	}

	*ind = (int*) malloc (N*sizeof(int));
	if (*ind == NULL){
		printMemoryAllocationError();
		exit(0);
	}

	*val = (double*) malloc (N*sizeof(double));
	if (*val == NULL){
		printMemoryAllocationError();
		exit(0);
	}
}

/*
 * free all memory allocation from the GRB model and environment
 */
void freeGurobi(GRBenv * env , GRBmodel * model , double * lb, char * vtype,
		char ** names, char * namestorage,  int * ind, double * val) {

	free (lb);
	free(vtype);
	free(names);
	free(namestorage);
	free(ind);
	free(val);

	/* Free model */
	GRBfreemodel(model);

	/* Free environment */
	GRBfreeenv(env);
}

/*
 * fill game->solvedBoard with the optimal solution values for each cell
 */
void fillSolvedBoard(Game* game, double* vals) {
	int i;
	int j;
	int k;

	int N = game->N;

	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			for (k = 0; k < N; k++) {
				/* solvedBoard[i][j] = k+1 iff vals[i*N*N + j*N + k] == 1.0 */
				if(vals[i*N*N + j*N + k] != 0.0) {
					game->solvedBoard[i][j] = k+1;
				}
			}
		}
	}
}

/*
 * Fill the arrays namestorage, lb, vtype, names
 * according to the game board.
 * These arrays are used by Gurobi to know which cells are already filled.
 */
void createEmptyGurobiModel(Game * game, char ** namestorage, double ** lb,
		char ** vtype, char *** names) {
	int i;
	int j;
	int k;
	char * cursor = NULL;
	int N = game->N;

	cursor = *namestorage;
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			for (k = 0; k < N; k++) {
				/* lb indicates the non Empty cells values in the supplied board */
				if (game->gameBoard[i][j]-1 == k) {
					(*lb)[i*N*N+j*N+k] = 1;
				}
				else {
					(*lb)[i*N*N+j*N+k] = 0;
				}
				/* variable types - Xijk is Binary (cell i*N*N+j*N+k in "vtype") */
				(*vtype)[i*N*N+j*N+k] = GRB_BINARY;
				(*names)[i*N*N+j*N+k] = cursor;
				/* give name to the variable Xijk */
				sprintf((*names)[i*N*N+j*N+k], "x[%d,%d,%d]", i, j, k+1);
				cursor += strlen((*names)[i*N*N+j*N+k]) + 1;
			}
		}
	}
}

/*
 * For every cell add the constraint that it must have exactly one value.
 * returns 1 iff an error occured
 */
int addCellConstraints(int N, GRBmodel * model, int ** ind, double ** val) {
	int i;
	int j;
	int k;
	int error;

	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			for (k = 0; k < N; k++) {
				(*ind)[k] = i*N*N + j*N + k;
				(*val)[k] = 1.0;
			}
			error = GRBaddconstr(model, N, *ind, *val, GRB_EQUAL, 1.0, NULL);
			if (error) {
				return 1;
			}
		}
	}
	return 0;
}

/*
 * For every col add the constraint that it must have all the different
 * values exactly once.
 * returns 1 iff an error occured
 */
int addColConstraints(int N, GRBmodel * model, int ** ind, double ** val){
	int i;
	int j;
	int k;
	int error;

	for (k = 0; k < N; k++) {
		for (j = 0; j < N; j++) {
			for (i = 0; i < N; i++) {
				(*ind)[i] = i*N*N + j*N + k;
				(*val)[i] = 1.0;
			}
			error = GRBaddconstr(model, N, *ind, *val, GRB_EQUAL, 1.0, NULL);
			if (error){
				return 1;
			}
		}
	}
	return 0;
}

/*
 * For every row add the constraint that it must have all the different
 * values exactly once.
 * returns 1 iff an error occured
 */
int addRowConstraints(int N, GRBmodel * model, int ** ind, double ** val){
	int i;
	int j;
	int k;
	int error;

	for (k = 0; k < N; k++) {
		for (i = 0; i < N; i++) {
			for (j = 0; j < N; j++) {
				(*ind)[j] = i*N*N + j*N + k;
				(*val)[j] = 1.0;
			}
			error = GRBaddconstr(model, N, *ind, *val, GRB_EQUAL, 1.0, NULL);
			if (error) {
				return 1;
			}
		}
	}
	return 0;
}

/*
 * For every block add the constraint that it must have all the different
 * values exactly once.
 * returns 1 iff an error occured
 */
int addBlockConstraints(int N, int m, int n, GRBmodel * model, int ** ind, double ** val){
	int i;
	int j;
	int k;
	int ig;
	int jg;
	int error;
	int count;

	for (k = 0; k < N; k++) {
		for (ig = 0; ig < n; ig++) {
			for (jg = 0; jg < m; jg++) {
				count = 0;
				for (i = ig * m; i < (ig+1) * m; i++) {
					for (j = jg * n; j < (jg+1) * n; j++) {
						(*ind)[count] = i*N*N + j*N + k;
						(*val)[count] = 1.0;
						count++;
					}
				}
				error = GRBaddconstr(model, N, *ind, *val, GRB_EQUAL, 1.0, NULL);
				if (error){
					return 1;
				}
			}
		}
	}
	return 0;
}

/*
 * add all the constraints of sudoku:
 * 1. Each cell gets a value
 * 2. Each value must appear once in each col
 * 3. Each value must appear once in each block
 * 4. Each value must appear once in each block
 * returns 1 iff an error occured
 */
int addConstraints(int N, int m, int n, GRBmodel * model, int ** ind, double ** val){
	int error;

	/* Constraint 1: Each cell gets a value */
	error = addCellConstraints(N, model, ind, val);
	if (error) {
		return 1;
	}

	/* Constraint 2: Each value must appear once in each col */
	error = addColConstraints(N, model, ind, val);
	if (error) {
		return 1;
	}

	/* Constraint 3: Each value must appear once in each row */
	error = addRowConstraints(N, model, ind, val);
	if (error) {
		return 1;
	}

	/* Constraint 4: Each value must appear once in each block*/
	error = addBlockConstraints(N, m, n, model, ind, val);
	if (error) {
		return 1;
	}

	return 0;
}

/*
 * optimize model and save optimal status to optimstatus.
 * returns 1 iff an error occured
 */
int optimizeModel(GRBmodel * model, int * optimstatus) {
	int error;

	/* Optimize model */
	error = GRBoptimize(model);
	if (error) {
		return 1;
	}

	/* Capture solution information */
	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, optimstatus);
	if (error) {
		return 1;
	}

	return 0;
}


/*
 * finds the indexes of the first empty cell
 * going up to down and left to right
 * and puts them in row and col
*/
void getFirstEmptyCell(Game * game, int * row, int *col) {
	int i;
	int j;
	int N = game->N;

	for(i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			if(game->gameBoard[i][j] == 0) {
				*row = i;
				*col = j;
				return;
			}
		}
	}
	*row = N;
	*col = N;
}



/* Solver.h implementation */

int solveBoardWithILP(Game* game){
	int optimstatus;
	double objval;
	double * lb = NULL;
	char * vtype = NULL;
	char ** names = NULL;
	char * namestorage = NULL;
	int  * ind = NULL;
	double  * val = NULL;
	GRBenv *env = NULL;
	GRBmodel *model = NULL;
	int error = 0;

	/* allocate all memory needed for gurobi */
	allocateGurobi (game->N, &lb, &vtype, &names, &namestorage,  &ind, &val);

	/* Create an empty model */
	createEmptyGurobiModel(game, &namestorage, &lb, &vtype, &names);

	/* Create environment */
	error = GRBloadenv(&env, "sudoku.log");
	if (error) {
		freeGurobi(env , model , lb, vtype, names, namestorage, ind, val);
		return 0;
	}

	/* Create new model */
	error = GRBnewmodel(env, &model, "sudoku", game->N*game->N*game->N,
						NULL, lb, NULL, vtype, names);
	if (error) {
		freeGurobi(env , model , lb, vtype, names, namestorage, ind, val);
		return 0;
	}

	/* add all constraints */
	error = addConstraints(game->N, game->m, game->n, model, &ind, &val);
	if (error) {
		freeGurobi(env , model , lb, vtype, names, namestorage, ind, val);
		return 0;
	}

	/* optimize model and capture solution information */
	error = optimizeModel(model, &optimstatus);
	if (error) {
		freeGurobi(env , model , lb, vtype, names, namestorage, ind, val);
		return 0;
	}

	/*
	 * check if boards is Solvable according to optimization status
	 * if it is - extract optimal solution to solvedBoard
	 */
	if (optimstatus == GRB_OPTIMAL) {
	    game->isSolvable = 1;
		error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &objval);
		if (error){
			freeGurobi(env , model , lb, vtype, names, namestorage, ind, val);
			return 0;
		}
		/* get the solution - the assignment to each variable */
		error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, game->N*game->N*game->N, lb);
		if (error){
			freeGurobi(env , model , lb, vtype, names, namestorage, ind, val);
			return 0;
		}
		/* fill solved board with the solution values */
		fillSolvedBoard(game, lb);
	}
	else if (optimstatus == GRB_INFEASIBLE) {
		game->isSolvable = 0;
	}
	else {
		freeGurobi(env , model , lb, vtype, names, namestorage, ind, val);
		return 0;
	}
	/* free memory allocation in the end of the function */
	freeGurobi(env , model , lb, vtype, names, namestorage, ind, val);
	return 1;
}

int cntNumSolutions(Game * game){
	int v;
	int val;
    int i = 0;
	int j = 0;
	int cntSoultions = 0;
	int N = game->N;

	/* Initialize stack */
	LinkedList * stack = createStack();

	/* get first empty cell */
	getFirstEmptyCell(game, &i, &j);

	/* insert the first empty cell to stack with value 1 */
	pushStack(stack, i, j, 1);

	/* run backtracking until stack is empty */
	while(isEmptyStack(stack) == 0) {

		/* get top item and pop it */
		topStack(stack, &i, &j, &val);
		popStack(stack);

		/* check if the board is filled */
		if(i == N) {

			/* increment soulutions counter */
			cntSoultions++;
			continue;
		}

		/* continue checking options for the current cell */
		for(v = val; v <= N; v++) {

			/* check if v is a legal option */
			game->gameBoard[i][j] = v;
			if (checkAndMarkBoardErrorsInCol(game, i, j, 0) == 0 &&
					checkAndMarkBoardErrorsInRow(game, i, j, 0) == 0 &&
					checkAndMarkBoardErrorsInBlock(game, i, j, 0) == 0) {

				/* insert cell with next option to stack */
				pushStack(stack, i, j, v+1);

				/* get next empty cell */
				getFirstEmptyCell(game, &i, &j);

				/* insert next cell with 1 to stack */
				pushStack(stack, i, j, 1);
				break;
			}
		}

		/* set cell back to 0 */
		if(v == N+1) {
			game->gameBoard[i][j] = 0;
		}
	}

	/* free stack and return result*/
	freeStack(stack);
	return cntSoultions;
}
