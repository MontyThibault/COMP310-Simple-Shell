#include "automaton.h"

struct automaton_state automaton_new(char *args[]) {
	struct automaton_state r;

	r.acceptance_state = t_unknown;
	r.args = args;
	r.current = &args[0];	

	return r;
}

int automaton_accepting(struct automaton_state *state) {
	if((*state).acceptance_state == t_accepting) {
		return 1;
	}

	return 0;
}

int automaton_failed(struct automaton_state *state) {
	if((*state).acceptance_state == t_failed) {
		return 1;
	}

	return 0;
}

struct automaton_state automaton_match(struct automaton_state state, AUTOMATON_MATCH_F f) {
	return f(state);
}
