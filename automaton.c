#include "automaton.h"

struct automaton_state automaton_new(char *args[]) {
	struct automaton_state r;

	r.acceptance_state = t_accepting;
	r.args = args;
	r.current = args;	

	return r;
}

void automaton_advance(struct automaton_state *state) {
	(*state).current += sizeof(char *);
}
