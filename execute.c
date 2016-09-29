#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

#include "execute.h"


void execute_generic(struct cmd_tagged_union *tagged_union) {
	if((*tagged_union).type == t_simple) {

		struct cmd_simple *const cmd = &(*tagged_union).value.u_simple;
		execute_cmd_simple(cmd);

	} else if((*tagged_union).type == t_pipe) {

		printf("Pipes not implemented yet.");
		struct cmd_compound *const cmd = &(*tagged_union).value.u_compound;
		execute_cmd_pipe(cmd);

	} else if((*tagged_union).type == t_redirect) {
		
		printf("Redirects not implemented yet.");
		struct cmd_compound *const cmd = &(*tagged_union).value.u_compound;
		execute_cmd_redirect(cmd);
	}
}


void execute_cmd_simple(struct cmd_simple *const cmd) {

	// cmd.background?
	int bg = 0;
	
	int pid = fork();
	
	if(pid == 0) {
	
		char *const args[ARG_ARRAY_LENGTH + 1];
		char *const program = cmd_extract_program(cmd);

		execvp(program, args);

		// This thread only continues running if execvp failed.
		printf("Invalid command name");
	}

	if(bg == 0) {
		
		int status;
		waitpid(pid, &status, 0);
	
	}
}


void execute_cmd_pipe(struct cmd_compound *cmd) {

}


void execute_cmd_redirect(struct cmd_compound *cmd) {

}
