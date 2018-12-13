
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Game.h"
#include "mainAux.h"

/*
 * Execute the command according to its type
 * checks is the command is available in the current game mode
 * and if it isn't - change the command type to INVALID so that the command is not executed
 */
void executeCommand(Command currCommand, Game * game){

    switch (currCommand.type){

        case SOLVE:{
			solve(game, currCommand);
            free(currCommand.filePath);
            break;
        }

        case EDIT:{
        	edit(game, currCommand);
        	if(currCommand.filePath != NULL){
        		free(currCommand.filePath);
        	}
            break;
        }

        case MARK_ERRORS:{
        	/*
        	 * mark_errors is only available in Solve mode.
        	 * it is Invalid in other modes and therefore a Invalid message is printed
        	 */
        	if (game->gameMode != SOLVE_MODE){
        		printInvalidCommandError();
        		return;
        	}
            markErrors(game, currCommand);
            break;
        }

        case PRINT_BOARD:{
        	/*
        	 *   a. Prints the board to the user.
        	 *	 b. print_board command is only available in Edit and Solve modes.
        	 *	 	Otherwise, treat it as an invalid command.
        	 */
        	if (game->gameMode != SOLVE_MODE && game->gameMode != EDIT_MODE ){
        		printInvalidCommandError();
        		return;
        	}
            printBoard(*game);
            break;
        }

        case SET:{
        	/*
        	 *	 Set command is only available in Edit and Solve modes.
        	 *	 	Otherwise, treat it as an invalid command.
        	 */
        	if (game->gameMode != SOLVE_MODE && game->gameMode != EDIT_MODE ){
        		printInvalidCommandError();
        		return;
        	}
            set(game, currCommand);
            break;
        }

        case VALIDATE:{
        	/*
			 *	 Validate command is only available in Edit and Solve modes.
			 *	 Otherwise, treat it as an invalid command.
			 */
			if (game->gameMode != SOLVE_MODE && game->gameMode != EDIT_MODE ){
				printInvalidCommandError();
				return;
			}
            validate(game);
            break;
        }

        case GENERATE:{
        	/*
        	 *  Generate command is only available in Edit mode.
			 *	Otherwise, treat it as an invalid command.
			 */
			if (game->gameMode != EDIT_MODE){
				printInvalidCommandError();
				return;
			}
            generate(game, currCommand);
            break;
        }

        case UNDO:{
        	/*
			 *	 Undo command is only available in Edit and Solve modes.
			 *	 Otherwise, treat it as an invalid command.
			 */
			if (game->gameMode != SOLVE_MODE && game->gameMode != EDIT_MODE ){
				printInvalidCommandError();
				return;
			}
            undo(game);
            break;
        }

        case REDO:{
        	/*
			 *	 Redo command is only available in Edit and Solve modes.
			 *	 Otherwise, treat it as an invalid command.
			 */
			if (game->gameMode != SOLVE_MODE && game->gameMode != EDIT_MODE ){
				printInvalidCommandError();
				return;
			}
            redo(game);
            break;
        }

        case SAVE:{
        	 /*	Save command is only available in Edit and Solve modes.
			 *	Otherwise, treat it as an invalid command.
			 */
			if (game->gameMode != SOLVE_MODE && game->gameMode != EDIT_MODE ){
				printInvalidCommandError();
				free(currCommand.filePath);
				return;
			}

			save(game, currCommand);
			free(currCommand.filePath);
            break;
        }

        case HINT:{
        	/*	hint command is only available in Solve mode.
			 *	Otherwise, treat it as an invalid command.
			 */
			if (game->gameMode != SOLVE_MODE){
				printInvalidCommandError();
				return;
			}
            hint(game, currCommand);
            break;
        }

        case NUM_SOLUTIONS:{
        	/*	num_solutions command is only available in Edit and Solve modes.
			 *	Otherwise, treat it as an invalid command.
			 */
			if (game->gameMode != SOLVE_MODE && game->gameMode != EDIT_MODE ){
				printInvalidCommandError();
				return;
			}
            numSolutions(game);
            break;
        }

        case AUTOFILL:{
        	/*
			 * Autofill is only available in Solve mode.
			 * it is Invalid in other modes and therefore a Invalid message is printed
			 */
			if (game->gameMode != SOLVE_MODE){
				printInvalidCommandError();
				return;
			}
            autofill(game);
            break;
        }

        case RESET:{
        	/*	reset command is only available in Edit and Solve modes.
			 *	Otherwise, treat it as an invalid command.
			 */
			if (game->gameMode != SOLVE_MODE && game->gameMode != EDIT_MODE ){
				printInvalidCommandError();
				return;
			}
            reset(game);
            break;
        }

        case EXIT:{
        	exitGame(game);
        	break;
        }

        case INVALID:{
            printInvalidCommandError();
            break;
        }

        case IGNORE:{
        	return;
        }
    }
}


void runGame(){

	Game game;
	/* buffer for user input */
	char * input;
	/* current command Command Struct */
	Command currCommand;

	/* Initialize new game */
	initalizeGameAtINIT(&game, 1);

    /* print the opening string */
	printStartSudokuString();

    while(game.gameOver == 0){

    	/* get command input from the user */
    	printEnterCommand();
        /*get command line from user
         * in case of a problem - print error and exit
         * Note: no need to exit cleanly
         */
    	input = getLine(stdin);
    	/*
    	 * if NULL was returned then we reached EOF
    	 * this is treated as an exit command
    	 */
    	if(input == NULL) {
    		currCommand.type = EXIT;
    		executeCommand(currCommand, &game);
    		continue;
    	}
    	/* if the command length exceed COMMAND_MAX_SIZE chars
    	 * treat it as invalid command
    	 */
    	if (strlen(input) > COMMAND_MAX_SIZE){
    		printInvalidCommandError();
    		free(input);
    		continue;
    	}

    	/* parse the user current command to the defined format - Command struct */
        currCommand = parseCommand(input);
        free(input);

        /* execute the parsed command by its type */
        executeCommand(currCommand, &game);

    }

}

