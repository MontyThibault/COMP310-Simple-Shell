#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

#include "execute.h"
#include "commands.h"
#include "builtin.h"


void execute_generic(struct cmd_tagged_union *tagged_union) {

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

void _branch_cmd_simple(struct cmd_simple *cmd) {
	
	char *args[ARG_ARRAY_LENGTH + 1];
	cmd_generate_ptr_arr(cmd, args);
	char *program_name = cmd_extract_program(cmd);

	if(builtin_match_and_run(program_name, args)) {
		exit(0);
	}

	execvp(program_name, args);

	// This thread only continues running if execvp failed.
	printf("Invalid command name");
	exit(0);
}

void execute_cmd_simple(struct cmd_simple *cmd) {

	int pid = fork();
	
	if(pid == 0) {
		_branch_cmd_simple(cmd);
	}

	if((*cmd).bg == 0) {
		int status;
		waitpid(pid, &status, 0);
	}
}

void _overwrite_stdout(int fd) {
	close(1);
	dup(fd);
}

void _overwrite_stdin(int fd) {
	close(0);
	dup(fd);
}

void execute_cmd_pipe(struct cmd_compound *cmd) {
	int pipefd[2];
	pipe(pipefd);

	int old_stdin = dup(0),
	    old_stdout = dup(1);

	int pid1 = fork();

	if(pid1 == 0) {
		close(pipefd[0]);
		_overwrite_stdout(pipefd[1]);
		_branch_cmd_simple(&(*cmd).cmd1);

	} else {
		int pid2 = fork();

		if(pid2 == 0) {
			close(pipefd[1]);
			_overwrite_stdin(pipefd[0]);
			_branch_cmd_simple(&(*cmd).cmd2);

		} else {

			close(pipefd[0]);
			close(pipefd[1]);

			int status;

			// Once the first process ends, kill the second
			waitpid(pid1, &status, 0);
			// kill(pid2, SIGTERM);
			waitpid(pid2, &status, 0);

			close(0);
			dup(old_stdin);
			
			close(1);
			dup(old_stdout);
		}
	}
}


void execute_cmd_redirect(struct cmd_compound *cmd) {
	printf("redirect");
}
