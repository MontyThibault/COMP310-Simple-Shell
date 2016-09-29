#include "commands.h"

enum automaton_acceptance_state {
	t_accepting,
	t_failed,
	t_unknown	
};

struct automaton_state {
	enum automaton_acceptance_state acceptance_state;
	struct cmd_tagged_union cmd;
	char **args;
	char **current;
};


struct automaton_state automaton_new(char *args[]);

int automaton_accepting(struct automaton_state *state);
int automaton_failed(struct automaton_state *state);


typedef struct automaton_state (*AUTOMATON_MATCH_F)(struct automaton_state);
struct automaton_state automaton_match(struct automaton_state state, AUTOMATON_MATCH_F); 
