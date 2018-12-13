# ConsoleSudokuGame
This is a console sudoku game that was the final project for the course "Software Project" at Tel Aviv University 2018.

The Sudoku project has the following functionalities:
- Solving Sudoku puzzles
- Editing and creating new Sudoku puzzles
- Supporting puzzles of any size
- Validating the board
- Saving and loading puzzles from files
- Randomly generating new puzzles
- Finding the number of solutions for a given board
- Auto-filling option for obvious cells (where only a single value is valid)

The program has two main modes: Edit and Solve, and an additional Init mode. In the Solve mode, the user attempts to solve a pre-existing puzzle. In the Edit mode, the user edits existing puzzles or creates new ones, and saves these for opening in the Solve mode later on. In the Init mode, the user loads a file to enter either Edit or Solve mode.

The project consists of 5 main parts:
- Sudoku game logic
- Console user interface
- ILP solver algorithm
- Random puzzle generator
- Exhaustive backtracking solver (for determining the number of different solutions)

The program interacts with the user via console interface, with the following commands:
1. **solve X** - Starts a puzzle in Solve mode, loaded from a file with the name "X", where X includes a full or relative path to the file.
2. **edit [X]** - Starts a puzzle in Edit mode, loaded from a file with the name "X", where X includes a full or relative path to the file.
   The parameter X is optional. If no parameter is supplied, the program should enter Edit mode with an empty board.
3. **mark_errors X** - Sets the "mark errors" setting to X, where X is either 0 or 1.
4. **print_board** - Prints the board to the user.
5. **set X Y Z** - Sets the value of cell <X,Y> to Z.
The user may empty a cell by setting Z=0.
6. **validate** - Validates the current board using ILP (Gurobi), ensuring it is solvable.
7. **generate X Y** - Generates a puzzle by randomly filling X cells with random legal values, running ILP to solve the resulting board, and then clearing all but Y random cells. This command is only available when the board is empty.
8. **undo** - Undo previous moves done by the user.
9. **redo** - Redo a move previously undone by the user.
10. **save X** - Saves the current game board to the specified file, where X includes a full or relative path to the file.
In Edit mode, the board is validated before saving.
11. **hint X Y** - Give a hint to the user by showing the solution of a single cell X,Y.
12. **num_solutions** - Print the number of solutions for the current board, using exhaustive backtracking algorithm.
13. **autofill** - Automatically fill "obvious" values – cells which contain a single legal value.
14. **reset** - Undo all moves, reverting the board to its original loaded state.
15. **exit** - Terminates the program. All memory resources are freed and all open files are closed.
