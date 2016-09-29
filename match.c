#include <string.h>
#include "match.h"

struct automaton_state match_end_of_input(struct automaton_state state) {
	if(**(state.current) != '\1') {
		state.acceptance_state = t_failed;
	}

	return state;
}

// Greedy
struct automaton_state match_optional(struct automaton_state state, AUTOMATON_MATCHER f) {
	struct automaton_state out = f(state);

	if(out.acceptance_state == t_accepting) {
		return out;
	} else {
		return state;
	}
}

// Greedy
struct automaton_state match_one_or_more(struct automaton_state state, AUTOMATON_MATCHER f) {
	
	struct automaton_state a = state;
	struct automaton_state b = f(a);

	// Returned a failed automaton_state if one match failed
	if(b.acceptance_state == t_failed) {
		return b;
	}
	
	while(b.acceptance_state == t_accepting) {
		a = b;
		b = f(a);
	}

	return a;
}

struct automaton_state match_const_token(struct automaton_state state, char *token) {
	char *current_string = *(state.current);

	if(strcmp(current_string, token) != 0) {
		state.acceptance_state &= t_accepting;
	} else {
		state.acceptance_state = t_failed;
	}

	automaton_advance(&state);

	return state;
}

struct automaton_state match_ampersand(struct automaton_state state) {
	struct automaton_state out = match_const_token(state, '&');

	if(out.acceptance_state == t_accepted) {
		state.cmd.value.u_simple.cmd.bg = 1;
	}

	return out;
}


struct automaton_state match_end(struct automaton_state state) {
	return match_end_of_input(match_optional(state, &match_ampersand));
}

struct automaton_state match_argument(struct automaton_state state) {
	char *arg = *(state.current);

	// Santitize arg to make sure it contains alpha-numeric characters, not longer than
	// ARG_LENGTH, etc.

	cmd_append_arg(&state.cmd.value.u_simple, arg);
	automaton_advance(&state);

	return state;
}

struct automaton_state match_command(struct automaton_state state) {
	
	state.cmd.type = t_simple;
	state.cmd.value.u_simple.len = 0;
	state.cmd.value.u_simple.bg = 0;
	
	return match_one_or_more(state, match_argument);
}

struct match_full_simple(struct automaton_state state) {
	return match_end(match_command(state));
}

struct match_full_compound(struct automaton_state state, char *delimeter) {	

	state = match_command(state);
	struct cmd_simple cmd1 = state.cmd.value.u_simple;	

	state = match_const_token(state, delimeter);

	state = match_command(state);
	struct cmd_simple cmd2 = state.cmd.value.u_simple;

	state =  match_end_of_input(state);

	state.cmd.value.u_compound.cmd1 = cmd1;
	state.cmd.value.u_compound.cmd2 = cmd2;

	return state;
}

struct automaton_state match_pipe(struct automaton_state state) {
	return match_full_compound(state, "|");
}

struct automaton_state match_redirect(struct automaton_state state) {
	return match_full_compound(state, ">");
}
