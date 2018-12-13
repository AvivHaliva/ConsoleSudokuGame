/***********************************************************************
* FILES NAMES :        Command.c , Command.h
*
* DESCRIPTION :
*       Command Parsing And Validation.
*
* PUBLIC FUNCTIONS :
*       Command parseCommand(char * stringOfCommand)
*
* NOTES :
*       1. Command.h contains the definition of the following:
*           a. commandType - an enum describing the different game legal command, as well as two extra commands:
*                i.  INVALID - describing an invalid command such as unsupported command or command with wrong syntax
*               ii. IGNORE - describing commands we should ignore such as blank lines
*           b. Command - a struct representing the desired fields of a valid command:
*                        command type, int arguments (if required) and file path (if required)
*           c. constants such as command max size and legal command names strings
*       2.  parseCommand function receives the user input string and tries to parse it to the defined struct, as described in function documentation
*       3.  The functions in Command.c support the parseCommand function with smaller tasks such as reading the command type and translating
*            it to the commandType enum, reading the supplied file path, reading, parsing and validating the supplied int arguments to the command, etc.
*
*
***********************************************************************/

/* command max size is 256 chars */
#define COMMAND_MAX_SIZE 256

/*
 * legal commands constant strings
 */
#define SOLVE_STR "solve"
#define EDIT_STR "edit"
#define MARK_ERROR_STR "mark_errors"
#define PRINT_BOARD_STR "print_board"
#define SET_STR "set"
#define VALIDATE_STR "validate"
#define GENERATE_STR "generate"
#define UNDO_STR "undo"
#define REDO_STR "redo"
#define SAVE_STR "save"
#define HINT_STR "hint"
#define NUM_SOLUTIONS_STR "num_solutions"
#define AUTOFILL_STR "autofill"
#define RESET_STR "reset"
#define EXIT_STR "exit"

/*
 * enum for all the legal commandType as described in the project info
 * INVALID stands for every illegal requested command
 * IGNORE stands for blank lines commands
 */

typedef enum commandType {SOLVE, EDIT, MARK_ERRORS, PRINT_BOARD, SET, VALIDATE,
	GENERATE, UNDO, REDO, SAVE, HINT, NUM_SOLUTIONS, AUTOFILL,
	RESET, EXIT, INVALID, IGNORE} commandType;

/*
 * Command struct will help us to keep track of the user commands.
 * each Command instance has:
 * 1. type: commandType - the request command
 * 2. intArgs: command (int) args supplied by user
 * 3. filePath : file path supplied by the user for EDIT, SAVE and SOLVE
 *
 */
typedef struct Command {
    /*
     * the type of the this command
     * */
    commandType type;

    /*
     * intArgs has the arguments for the command that are integers
     * for example: in set <x,y,z> x will be in the 0 element place
     * y will be in the 1 element place and z will be int the 2 element place of the array
     * */
    int intArgs[3];

    /*
     * used for the commands EDIT and SOLVE to load a file from this location
     * used for the command SAVE to save the file to this location
     * */
    char * filePath;

} Command;


/*
 * gets command input string from the user
 * return the command parsed to:
 * 1. command Type
 * 2. command Arguments - int and filepath (if needed)
 *
 * in case of invalid command - update the command Type to INVALID
 * (invalid command, command with a wrong syntax or parameters)
 *
 * Note:
 * 1.	extra parameters are OK
 * 2. 	command may not span two lines -
 * 			it will be considered as 2 different commands
 * 3. 	any input line beyond 256 chars is invalid
 * 4.	parseCommand function is NOT responsible to check
 * 								if the requested command is available in the current game mode
 * 5. command type is NOT case sensitive
 *
 */
Command parseCommand(char * stringOfCommand);
