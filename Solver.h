
/************************************
* FILES NAMES :        Solver.c , Solver.h
*
* DESCRIPTION :
*       Solve the sudoku game in different techniques using ILP solver algorithm (Gurobi) and Exhaustive backtracking solver.
*
*
* PUBLIC FUNCTIONS :
*      int solveBoardWithILP(Game* game)
*      int cntNumSolutions(Game* game)
*
* NOTES :
*   1. cntNumSolutions using Exhaustive BT with an implicit stack instead of recursion.
*      The function returns the game num of solutions, without changing the board.
*   2. solveBoardWithILP using the ILP solver algorithm (Gurobi) to check if the board is solvable.
*      In this case, the return value indicate if the solver has worked or there was a problem during the execution.
*      The function set isSolvable to 1 if it the game was found solvable in the ILP solver run.
*
************************************/

 /*
  * Copyright 2017, Gurobi Optimization, Inc.
  * Note: This code is based on an example from the gurubi website
  *
  * The Sudoku board is a NxN grid, which is further divided into a mxn block
  * of mxn blocks. Each cell in the grid must take a value from 1 to 9.
  * No two grid cells in the same row, column, or bloc may take the
  * same value.
  * The binary variables x[i,j,k] indicate whether
  * cell <i,j> takes value 'k'.  The constraints are as follows:
  *  1. Each cell must take exactly one value (sum_v x[i,j,k] = 1)
  *  2. Each value is used exactly once per row (sum_i x[i,j,k] = 1)
  *  3. Each value is used exactly once per column (sum_j x[i,j,k] = 1)
  *  4. Each value is used exactly once per mxn block (sum_grid x[i,j,k] = 1)
  *
  *  The function gets a pointer to the game and returns an int
  *  which indicates if the ILP solution worked without a problem (1)
  *  or there was a problem during the execution (0).
  *
  * In case there wasn't a problem with the execution -
  * the function also changes game isSolvable attribute
  * to 1 in case there is an optimal solution or to 0 if there isn't such solution
*/

int solveBoardWithILP(Game* game);

/*
 *  The exhaustive backtracking algorithm doesn't finish when the last cell is filled with a legal value.
 *  Instead, it increments a counter, and then continues to increment the value of that cell, 
 *  or backtrack if necessary in order to retrieve other potential solutions.
 *  Thus, the algorithm exhausts all possible values for all empty cells of the board, counting all solutions found in this process. 
 *  Once the algorithm backtracks from the 1st empty cell, the algorithm is finished, 
 *  and the counter contains the number of different solutions available for the current board and the function returns it.
 *  The exhaustive backtracking algorithm  is implemented with an explicit stack for simulating recursion, rather than through recursive calls.
 */
int cntNumSolutions(Game* game);


