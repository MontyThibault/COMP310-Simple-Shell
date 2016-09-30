#include <string.h>
#include <stdio.h>
#include "commands.h"
#include "match.h"
#include "history.h"

struct automaton_state match_end_of_input(struct automaton_state state) {
	if(**(state.current) != '\0') {
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

	if(b.acceptance_state == t_failed) {
		state.acceptance_state = t_failed;
		return state;
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
		state.acceptance_state = t_failed;
	}

	automaton_advance(&state);

	return state;
}

struct automaton_state match_ampersand(struct automaton_state state) {
	struct automaton_state out = match_const_token(state, "&");

	state.cmd.value.u_simple.bg = 1;

	return out;
}


struct automaton_state match_end(struct automaton_state state) {
	return match_end_of_input(match_optional(state, &match_ampersand));
}

struct automaton_state match_argument(struct automaton_state state) {

	// Santitize arg to make sure it contains alpha-numeric characters, not longer than
	// ARG_LENGTH, etc.

	struct automaton_state failure_conditions[3];

	if(match_end_of_input(state).acceptance_state == t_accepting) {
		state.acceptance_state = t_failed;
		return state;
	}

	if(match_const_token(state, "&").acceptance_state == t_accepting
	|| match_const_token(state, "|").acceptance_state == t_accepting) {
		state.acceptance_state = t_failed;
		return state;
	}

	cmd_append_arg(&state.cmd.value.u_simple, *(state.current));
	automaton_advance(&state);

	return state;
}

struct automaton_state match_command(struct automaton_state state) {
	
	state.cmd.type = t_simple;
	state.cmd.value.u_simple.len = 0;
	state.cmd.value.u_simple.bg = 0;
	
	return match_one_or_more(state, match_argument);
}

struct automaton_state match_history(struct automaton_state state) {
	
	char *arg = *state.current;
	int hist_number;

	// I should have just used this the whole time instead of the automaton business.
	// That would have been SO MUCH easier.
	if(sscanf(arg, "!%d", &hist_number) == 1) {
		state.cmd = *hist_fetch(hist_number);
	} else {
		state.acceptance_state = t_failed;
	}

	automaton_advance(&state);

	return state;
}

struct automaton_state match_full_history(struct automaton_state state) {
	return match_end_of_input(match_history(state));
}

struct automaton_state match_full_simple(struct automaton_state state) {
	return match_end(match_command(state));
}

struct automaton_state match_full_compound(struct automaton_state state, char *delimeter, enum cmd_type out_type) {	

	state = match_command(state);
	struct cmd_simple cmd1 = state.cmd.value.u_simple;	

	state = match_const_token(state, delimeter);

	state = match_command(state);
	struct cmd_simple cmd2 = state.cmd.value.u_simple;

	state =  match_end_of_input(state);

	state.cmd.type = out_type;
	state.cmd.value.u_compound.cmd1 = cmd1;
	state.cmd.value.u_compound.cmd2 = cmd2;

	return state;
}

struct automaton_state match_full_pipe(struct automaton_state state) {
	return match_full_compound(state, "|", t_pipe);
}

struct automaton_state match_full_redirect(struct automaton_state state) {
	return match_full_compound(state, ">", t_redirect);
}

struct automaton_state match_full_generic(struct automaton_state state) {

	struct automaton_state after_matching;

	// Empty?

	if((after_matching = match_full_history(state)).acceptance_state == t_accepting) {
		return after_matching;
	}

	if((after_matching = match_full_simple(state)).acceptance_state == t_accepting) {
		return after_matching;
	}

	if((after_matching = match_full_pipe(state)).acceptance_state == t_accepting) {
		return after_matching;
	}

	if((after_matching = match_full_redirect(state)).acceptance_state == t_accepting) {
		return after_matching;
	}
	
	// Return failed state
	return after_matching;
}
