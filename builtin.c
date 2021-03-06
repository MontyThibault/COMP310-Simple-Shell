#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>

#include "builtin.h"
#include "history.h"
#include "jobs.h"


void builtin_history() {
	hist_print();
}

void builtin_pwd() {
	#define CWD_BUFFER_SIZE 100
	
	char buff[CWD_BUFFER_SIZE];
	getcwd(buff, CWD_BUFFER_SIZE);

	printf("%s", buff);
}

void builtin_cd(char *args[]) {

	int x = chdir(args[1]);

	if(x != 0) {
		perror("Error in cd");
	}
}

void builtin_exit() {
	// pid_t parent_pid = getppid();
	// kill(parent_pid, SIGTERM);
	exit(0);
}


int builtin_match_and_run(char *program_name, char *args[]) {
	if(strcmp(program_name, "history") == 0) {
		builtin_history();
		return 1;
	}

	if(strcmp(program_name, "pwd") == 0) {
		builtin_pwd();	
		return 1;
	}

	if(strcmp(program_name, "cd") == 0) {
		builtin_cd(args);
		return 1;
	}

	if(strcmp(program_name, "exit") == 0) {
		builtin_exit();
		return 1;
	}

	if(strcmp(program_name, "jobs") == 0) {
		print_jobs();
		return 1;
	}

	if(strcmp(program_name, "fg") == 0) {
		int job_number;
		if(args[1] == 0 || sscanf(args[1], "%d", &job_number) == 0)
			printf("Invalid argument to fd");
		else
			foreground_job(job_number);

		return 1;
	}

	return 0;
}
