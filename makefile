flags = -g -std=c99


all: test shell

commands.o: commands.c commands.h
	gcc $(flags) -c commands.c

history.o: history.c history.h commands.o
	gcc $(flags) -c history.c commands.o

execute.o: execute.c execute.h commands.o
	gcc $(flags) -c execute.c commands.o

parse.o: parse.c parse.h commands.o
	gcc $(flags) -c parse.c commands.o

automaton.o: automaton.c automaton.h commands.o
	gcc $(flags) -c automaton.c commands.o

test: automaton.o history.o commands.o execute.o parse.o test.c
	gcc $(flags) test.c history.o commands.o execute.o parse.o -o test

minunit.o: minunit.h
	gcc $(flags) -c minunit.h 


clear:
	rm *.o shell test a.out
