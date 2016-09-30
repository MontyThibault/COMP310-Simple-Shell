flags = -g

all: test shell

commands.o: commands.c commands.h
	gcc $(flags) -c commands.c

history.o: history.c history.h commands.h
	gcc $(flags) -c history.c 

execute.o: execute.c execute.h commands.h
	gcc $(flags) -c execute.c 

parse.o: parse.c parse.h commands.h
	gcc $(flags) -c parse.c 

automaton.o: automaton.c automaton.h commands.h
	gcc $(flags) -c automaton.c 

match.o: match.c match.h history.h
	gcc $(flags) -c match.c 

test: automaton.o match.o history.o commands.o execute.o parse.o test.c
	gcc $(flags) test.c automaton.o match.o history.o commands.o execute.o parse.o -o test

shell: automaton.o match.o history.o commands.o execute.o parse.o shell.c
	gcc $(flags) shell.c automaton.o match.o history.o commands.o execute.o parse.o -o shell


minunit.o: minunit.h
	gcc $(flags) -c minunit.h 


clear:
	rm *.o shell test a.out
