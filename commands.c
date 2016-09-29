#include <string.h>

#include "commands.h"


struct cmd_simple *cmd_extract_fst(struct cmd_compound *cmd) {
	return &((*cmd).cmd1);
}

struct cmd_simple *cmd_extract_snd(struct cmd_compound *cmd) {
	return &((*cmd).cmd2);
}

char *cmd_extract_arg_at(struct cmd_simple *cmd, int index) {
	return &((*cmd).args[index][0]);
}

char *cmd_extract_program(struct cmd_simple *cmd) {
	return cmd_extract_arg_at(cmd, 0);
}

// Writes strings to the internal string buffer from an array of char*'s.
void cmd_copy_from_ptr_arr(struct cmd_simple *cmd, char *arr[], int len) {
	(*cmd).len = len;

	for(int i = 0; i < len; i++) {

		char *arg = cmd_extract_arg_at(cmd, i);	
		strcpy(arg, arr[i]);
	}
}


// Generates an array of char *'s pointing into the internal string buffer.
void cmd_generate_ptr_arr(struct cmd_simple *cmd, char *arr[]) {
	for(int i = 0; i < (*cmd).len; i++) {
	
		arr[i] = cmd_extract_arg_at(cmd, i);

	}

	// Terminate so that execvp knows when to stop!
	arr[(*cmd).len] = '\0';	
}
