#include "jobs.h"
#include <stdio.h>
#include <string.h>

struct cmd_simple jobs_array[MAX_JOBS];

// Array index of last added job
int job_counter = -1;

// Returns -1 on fail, otherwise returns array index
int add_job(struct cmd_simple cmd) {
	if(job_counter >= MAX_JOBS - 1) {
		return -1;
	}

	job_counter++;
	jobs_array[job_counter] = cmd;

	return job_counter;
}

void remove_job_by_cmd(struct cmd_simple *cmd) {
	for(int i = 0; i <= job_counter; i++) {
	
		if(memcmp(&jobs_array[i], cmd, sizeof(struct cmd_simple)) == 0) {
			remove_job_by_index(i);
			break;
		}
	}
}

void remove_job_by_index(int index) {
	
	for(int i = index; i < MAX_JOBS - 1; i++) {
		jobs_array[i] = jobs_array[i + 1];
	}
	
	job_counter--;
}

void print_jobs() {
	for(int i = 0; i <= job_counter; i++) {
		char *program_name = cmd_extract_program(&jobs_array[i]);
		printf("%d %s\n", i+1, program_name);
	}
}


void foreground_job(int index) {
	struct cmd_simple my_job = jobs_array[index];
	remove_job_by_index(index);
	add_job(my_job);
	// How does this actually work?
}
