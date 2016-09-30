#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

#include "execute.h"


void execute_generic(struct cmd_tagged_union *tagged_union) {

	// Built-ins

	if((*tagged_union).type == t_simple) {

		struct cmd_simple *const cmd = &(*tagged_union).value.u_simple;
		execute_cmd_simple(cmd);

	} else if((*tagged_union).type == t_pipe) {

		struct cmd_compound *const cmd = &(*tagged_union).value.u_compound;
		execute_cmd_pipe(cmd);

	} else if((*tagged_union).type == t_redirect) {
		
		struct cmd_compound *const cmd = &(*tagged_union).value.u_compound;
		execute_cmd_redirect(cmd);
	}
}


void execute_cmd_simple(struct cmd_simple *cmd) {

	int pid = fork();
	
	if(pid == 0) {
	
		char *args[ARG_ARRAY_LENGTH + 1];
		cmd_generate_ptr_arr(cmd, args);
		char *program = cmd_extract_program(cmd);

		execvp(program, args);

		// This thread only continues running if execvp failed.
		printf("Invalid command name");
	}

	if((*cmd).bg == 0) {
		
		int status;
		waitpid(pid, &status, 0);
	
	}
}


void execute_cmd_pipe(struct cmd_compound *cmd) {
	printf("pipe");
}


void execute_cmd_redirect(struct cmd_compound *cmd) {
	printf("redirect");
}
