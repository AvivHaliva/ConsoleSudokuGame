/***********************************************************************
* FILES NAMES :        GameFlow.c , GameFlow.h
*
* DESCRIPTION :
*       Initialize new game parameters and Manage the main sudoku game loop:
*       get user commands, send them to the parser and check their availability, as well as sending each command to execution with the desired parameters.
*
* PUBLIC FUNCTIONS :
*       void runGame()

* NOTES :
*       1. runGame function loop is executed while the game is not over
*       2. runGame use the internal function, executeCommand in order to check the requested command availability in the current game mode.
*          if the requested command is indeed available - executeCommand call the suitable function from Game.h with the desired parameters.
*
***********************************************************************/

/*
 * Initialize a new game parameters and starts the game loop
 */
void runGame();







