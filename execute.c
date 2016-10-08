#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>

#include "execute.h"
#include "commands.h"
#include "builtin.h"
#include "jobs.h"


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

	execvp(program_name, args);

	printf("Invalid command name");
	exit(0);
}

struct _waiting_thread_args {
	int pid;
	struct cmd_simple cmd;
};

void *_spawn_waiting_thread(void *args) {
	struct _waiting_thread_args *good_args_ptr = (struct _waiting_thread_args*) args;
	struct _waiting_thread_args good_args = *good_args_ptr;

	int status;
	waitpid(good_args.pid, &status, 0);

	printf("Job %s terminated", cmd_extract_program(&good_args.cmd));

	remove_job_by_cmd(&good_args.cmd);
	free(good_args_ptr);
}

void execute_cmd_simple(struct cmd_simple *cmd) {

	char *args[ARG_ARRAY_LENGTH + 1];
	cmd_generate_ptr_arr(cmd, args);
	char *program_name = cmd_extract_program(cmd);

	if(builtin_match_and_run(program_name, args)) {
		return;
	}

	int pid = fork();
	
	if(pid == 0) {
		_branch_cmd_simple(cmd);
	}
	

	if((*cmd).bg == 0) {
		int status;
		waitpid(pid, &status, 0);
	} else {

		// Add pid to current jobs list
		int index = add_job(*cmd);
		
		if(index == -1) {
			printf("Error: jobs array full!");
			return;
		}

		// On job die, remove it
		// We have a special thread that waits just for this job.
		// This has to be a thread and not a process because it
		// modifies OUR jobs array.
		
		// The arguments must be heap-allocated, since we aren't bound to this stack anymore
		struct _waiting_thread_args *args = (struct _waiting_thread_args*) malloc(sizeof(struct _waiting_thread_args));
		(*args).pid = pid;
		(*args).cmd = *cmd;

		pthread_t thread;
		pthread_create(&thread, 0, &_spawn_waiting_thread, (void*) args);
		pthread_detach(thread);
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

	char *program_name = cmd_extract_program(&(*cmd).cmd2);
	int file = open(program_name, O_WRONLY);
	
	int pid = fork();

	if(pid == 0) {
		_overwrite_stdout(file);
		_branch_cmd_simple(&(*cmd).cmd1);
	}
	
	close(file);
}
