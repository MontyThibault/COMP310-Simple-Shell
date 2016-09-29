#include "commands.h"

#ifndef INCLUDE_COMMANDS
#define INCLUDE_COMMANDS

typedef int automaton_acceptance_state;
#define t_accepting 1
#define t_failed 0

struct automaton_state {
	automaton_acceptance_state acceptance_state;
	struct cmd_tagged_union cmd;
	char **args;
	char **current;
};


struct automaton_state automaton_new(char *args[]);
void automaton_advance(struct automaton_state *state);
typedef struct automaton_state (*AUTOMATON_MATCHER)(struct automaton_state);

#endif
