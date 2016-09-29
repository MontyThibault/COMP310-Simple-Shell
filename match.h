#include "automaton.h"

struct automaton_state match_end_of_input(struct automaton_state state);

// Used internally
struct automaton_state match_optional(struct automaton_state state, AUTOMATON_MATCHER f);
struct automaton_state match_one_or_more(struct automaton_state state, AUTOMATON_MATCHER f);

struct automaton_state match_const_token(struct automaton_state state, char *token);
struct automaton_state match_ampersand(struct automaton_state state);

// Optional ampersand followed by line end
struct automaton_state match_end(struct automaton_state state);

struct automaton_state match_argument(struct automaton_state state);
// struct.cmd will be overwritten with a cmd_simple containing the command
struct automaton_state match_command(struct automaton_state state);


// The following matches entire inputs (and are really the only module exports)
struct automaton_state match_full_simple(struct automaton_state state);
struct automaton_state match_full_compound(struct automaton_state state, char *delimeter, enum cmd_type out_type);
struct automaton_state match_full_pipe(struct automaton_state state);
struct automaton_state match_full_redirect(struct automaton_state state);
