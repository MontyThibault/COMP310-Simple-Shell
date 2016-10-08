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

void _overwrite_stdout(int fd) {
	close(1);
	dup(fd);
}

void _overwrite_stdin(int fd) {
	close(0);
	dup(fd);
}

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

void *_spawn_waiting_thread(void *cmd_heap) {
	struct cmd_simple *cmd_ptr = (struct cmd_simple*) cmd_heap;
	struct cmd_simple cmd = *cmd_ptr;

	int status;
	waitpid(cmd.pid, &status, 0);

	printf("(Job %s exited)\n", cmd_extract_program(&cmd));

	remove_job_by_cmd(&cmd);
	free(cmd_ptr);
}

void execute_cmd_simple(struct cmd_simple *cmd) {

	char *args[ARG_ARRAY_LENGTH + 1];
	cmd_generate_ptr_arr(cmd, args);
	char *program_name = cmd_extract_program(cmd);

	if(builtin_match_and_run(program_name, args)) {
		return;
	}

	(*cmd).pid = fork();
	
	// Background process?	
	if((*cmd).bg == 0) {
		if((*cmd).pid == 0) {
			_branch_cmd_simple(cmd);
		}

		int status;
		waitpid((*cmd).pid, &status, 0);

	} else {
		
		// Create stdin redirection mechanism	
		int pipe_fd[2];
		pipe(pipe_fd);

		int read_fd = pipe_fd[0],
		    write_fd = pipe_fd[1];
		
		if((*cmd).pid == 0) {
			close(write_fd);
			_overwrite_stdin(read_fd);
			_branch_cmd_simple(cmd);

		}

		(*cmd).write_fd = write_fd;


		// Add pid to current jobs list
		int index = add_job(*cmd);
		
		if(index == -1) {
			printf("Error: jobs array full!");
			return;
		}

		// On job die, remove it from job list
		// We have a special thread that waits just for this job.
		// This has to be a thread and not a process because it
		// modifies OUR jobs array.
		
		// The arguments must be heap-allocated, since we aren't bound to this stack anymore
		struct cmd_simple *cmd_heap = (struct cmd_simple*) malloc(sizeof(struct cmd_simple));
		(*cmd_heap) = *cmd;

		pthread_t thread;
		pthread_create(&thread, 0, &_spawn_waiting_thread, (void*) cmd_heap);
		pthread_detach(thread);
	}
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
	
			// Bug-prone?
			waitpid(pid1, &status, 0);
			waitpid(pid2, &status, 0);

			// close(0);
			// dup(old_stdin);
			
			// close(1);
			// dup(old_stdout);
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
