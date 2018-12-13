CC = gcc
OBJS = main.o mainAux.o Game.o Command.o GameFlow.o LinkedList.o Solver.o
EXEC = sudoku-console
COMP_FLAG = -ansi -Wall -Wextra -Werror -pedantic-errors
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56

all: $(EXEC)
$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(GUROBI_LIB) -o $@ -lm
main.o: main.c Game.h GameFlow.h 
	$(CC) $(COMP_FLAG) -c $*.c
mainAux.o: mainAux.c 
	$(CC) $(COMP_FLAG) -c $*.c
Game.o: Game.c mainAux.h Game.h Solver.h LinkedList.h
	$(CC) $(COMP_FLAG) -c $*.c
GameFlow.o: GameFlow.c mainAux.h Game.h Command.h LinkedList.h
	$(CC) $(COMP_FLAG) -c $*.c
Command.o: Command.c Command.h mainAux.h
	$(CC) $(COMP_FLAG) -c $*.c
LinkedList.o: LinkedList.c LinkedList.h
	$(CC) $(COMP_FLAG) -c $*.c
Solver.o: Solver.c
	$(CC) $(COMP_FLAG) $(GUROBI_COMP) -c $*.c
clean:
	rm -f $(OBJS) $(EXEC)