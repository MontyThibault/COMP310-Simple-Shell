#include <string.h>
#include <stdio.h>
#include "commands.h"
#include "history.h"
#include "automaton.h"
#include "match.h"
#include "execute.h"
#include "parse.h"


int main(int argc, char *argv[]) {

	char *args[ARG_ARRAY_LENGTH];
	char stdin_string[STDIN_BUFFER_SIZE];

	while(1) {
		int len = getcmd("\n>> ", args, &stdin_string[0]);

		struct automaton_state state = automaton_new(args);	

		if(match_end_of_input(state).acceptance_state == t_accepting) {
			continue;
		}

		state = match_full_generic(state);

		if(state.acceptance_state == t_failed) {
			printf("Malformed input");
			continue;
		}

		// Copy orginal string into cmd struct
		strcpy(state.cmd.stdin_string, stdin_string);
	
		execute_generic(&state.cmd);	
		hist_add(state.cmd);
	}

	return 0;
}
