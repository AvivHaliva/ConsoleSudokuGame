
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "Command.h"
#include "mainAux.h"

/*
 * get commandType as typed by the user
 * convert it to the requested commandType (enum)
 * if the command is unknown - the returned value will be INVALID
 */
commandType getCurrCommandType (char * commandTypeStr){
	if (strcmp(commandTypeStr, SOLVE_STR) == 0){
		return SOLVE;
	}
	if (strcmp(commandTypeStr, EDIT_STR) == 0){
		return EDIT;
	}
	if (strcmp(commandTypeStr, MARK_ERROR_STR) == 0){
		return MARK_ERRORS;
	}
	if (strcmp(commandTypeStr, PRINT_BOARD_STR) == 0){
		return PRINT_BOARD;
	}
	if (strcmp(commandTypeStr, SET_STR) == 0){
		return SET;
	}
	if (strcmp(commandTypeStr, VALIDATE_STR) == 0){
		return VALIDATE;
	}
	if (strcmp(commandTypeStr, GENERATE_STR) == 0){
		return GENERATE;
	}
	if (strcmp(commandTypeStr, UNDO_STR) == 0){
		return UNDO;
	}
	if (strcmp(commandTypeStr, REDO_STR) == 0){
		return REDO;
	}
	if (strcmp(commandTypeStr, SAVE_STR) == 0){
		return SAVE;
	}
	if (strcmp(commandTypeStr, HINT_STR) == 0){
		return HINT;
	}
	if (strcmp(commandTypeStr, NUM_SOLUTIONS_STR) == 0){
		return NUM_SOLUTIONS;
	}
	if (strcmp(commandTypeStr, AUTOFILL_STR) == 0){
		return AUTOFILL;
	}
	if (strcmp(commandTypeStr, RESET_STR) == 0){
		return RESET;
	}
	if (strcmp(commandTypeStr, EXIT_STR) == 0){
		return EXIT;
	}
	return INVALID;
}

/*
 * gets a string and checks if all of its chars are digits
 * return 1 iff all of the string chars are digits
 */
int isLegalInt(char * str) {
	int i = 0;
	while(str[i] != '\0'){
		if (!isdigit(str[i])){
			return 0;
		}
		i++;
	}
	return 1;
}

/*
 * input:
 * 1. commandArray - the command String after we broke it to a series of tokens
 * 2. parsedCommand - the Command struct in order to fill in the desired int params
 *
 * the function parse only the desired arguments for each command Type.
 * a. in case a desired argument is missing -
 * 			we change the command type to INVALID
 * b. if the argument is not an int - we change its value to -1
 */
void extractCommandArgs (char * commandArray, Command * parsedCommand){

	char * tempArg;

	/*
	 * get the first param and save it as the first element of intArgs array
	 * if the user didn't supply such param - change command type to INVALID
	 */
	commandArray = strtok(NULL, DELIMITER);
	if (commandArray != NULL){
		tempArg =  commandArray;
		if (isLegalInt(tempArg) == 1){
			parsedCommand->intArgs[0] = atoi(tempArg);
		}
		else {
			parsedCommand->intArgs[0] = -1;
		}
		commandArray = strtok(NULL, DELIMITER);
	}
	else {
		parsedCommand->type = INVALID;
		return;
	}

	/*
	 * get the second param and save it as the second element of intArgs array
	 * if the user didn't supply such param - change command type to INVALID
	 * irrelevant for Mark_Errors (only 1 param is needed)
	 */
	if (parsedCommand->type != MARK_ERRORS){
		if (commandArray != NULL){
			tempArg =  commandArray;
			if (isLegalInt(tempArg) == 1){
				parsedCommand->intArgs[1] = atoi(tempArg);
			}
			else {
				parsedCommand->intArgs[1] = -1;
			}
			commandArray = strtok(NULL, DELIMITER);
		}
		else {
			parsedCommand->type = INVALID;
			return;
		}
	}

	/*
	 * get the third param and save it as the second element of intArgs array
	 * if the user didn't supply such param - change command type to INVALID
	 * relevant only for SET command (3 params are needed)
	 */
	if (parsedCommand->type == SET){
		if (commandArray != NULL){
			tempArg =  commandArray;
			if (isLegalInt(tempArg) == 1){
				parsedCommand->intArgs[2] = atoi(tempArg);
			}
			else {
				parsedCommand->intArgs[2] = -1;
			}
		}
		else {
			parsedCommand->type = INVALID;
			return;
		}
	}

}

/*
 * 1. commandArray - the command String after we broke it to a series of tokens
 * 2. parsedCommand - the Command struct in order to fill in the desired filePath param
 *
 * the function gets the filepath as supplied by the user.
 * if no filePath was supplied and the command is Save or Solve -
 * 		a desired info is missing so we change the command type to INVALID
 */
void extractCommandFilePath(char * commandArray, Command * parsedCommand){
	commandArray = strtok(NULL, DELIMITER);
	/* if a file path was supplied - allocate memory for it and update command->filePath */
	if (commandArray != NULL){
		parsedCommand->filePath = (char*) malloc(strlen(commandArray)+1);
		if(parsedCommand->filePath == NULL) {
			printMemoryAllocationError();
			exit(0);
		}
		strcpy(parsedCommand->filePath, commandArray);
	}

	else {
		/* if the command type is not EDIT - the command is not valid
		 * because a desired information (filePath) is missing
		 */
		parsedCommand->filePath = NULL;
		if (parsedCommand->type != EDIT){
			parsedCommand->type = INVALID;
		}
	}
}

/* gets a string and convert it to lower case representation */
void toLower(char * string){
	   int c = 0;
	   while (string[c] != '\0') {
		   string[c] = tolower(string[c]);
		   c++;
	   }
}

Command parseCommand(char * stringOfCommand){
	/* curr command struct instance */
	Command parsedCommand;
	/* empty string that will hold the command after we we will break it to tokens */
	char * commandArr;
	/* tmp string to hold command type string before converting it to the enum values */
	char * commandTypeStr;

    /*split the command line to string array*/
	commandArr = strtok(stringOfCommand, DELIMITER);

	/*
	 * Ignore blank lines
	 * (i.e., empty lines or lines that contain only whitespace chars)
	 */
	if(commandArr == NULL){
		parsedCommand.type = IGNORE;
		return parsedCommand;
	}

	/* user command type input will be converted to lower case representation */
	toLower(commandArr);
	commandTypeStr = commandArr;

	/*
	 * get the commandType for the current commands as one of the
	 * enum values options
	 */
	parsedCommand.type = getCurrCommandType(commandTypeStr);

	/*
	 * extract desired int args for the following commands :
	 * mark_errors x, set x y z, hint x y, generate x y
	 */
	if (parsedCommand.type == MARK_ERRORS ||
			parsedCommand.type == SET ||
			parsedCommand.type == HINT ||
			parsedCommand.type == GENERATE){
				extractCommandArgs(commandArr, &parsedCommand);
	}

	/*
	 * extract File path for the following commands :
	 * solve, save , edit (File path is optional)
	 */
	if (parsedCommand.type == SOLVE ||
			parsedCommand.type == EDIT ||
			parsedCommand.type == SAVE){
				extractCommandFilePath(commandArr, &parsedCommand);
	}

	return parsedCommand;
}

