flags = -g -pthread

all: test shell

commands.o: commands.c commands.h
	gcc $(flags) -c commands.c

history.o: history.c history.h commands.h
	gcc $(flags) -c history.c 

builtin.o: builtin.c builtin.h jobs.h
	gcc $(flags) -c builtin.c

execute.o: execute.c execute.h commands.h builtin.h
	gcc $(flags) -c execute.c 

parse.o: parse.c parse.h commands.h
	gcc $(flags) -c parse.c 

automaton.o: automaton.c automaton.h commands.h
	gcc $(flags) -c automaton.c 

match.o: match.c match.h history.h
	gcc $(flags) -c match.c 

jobs.o: jobs.c jobs.h commands.h
	gcc $(flags) -c jobs.c

test: automaton.o match.o history.o builtin.o commands.o execute.o parse.o test.c jobs.o
	gcc $(flags) test.c automaton.o match.o builtin.o history.o commands.o execute.o jobs.o parse.o -o test

shell: automaton.o match.o history.o builtin.o commands.o execute.o parse.o shell.c jobs.o
	gcc $(flags) shell.c automaton.o match.o builtin.o history.o commands.o execute.o jobs.o parse.o -o shell


minunit.o: minunit.h
	gcc $(flags) -c minunit.h 


clear:
	rm *.o shell test a.out
