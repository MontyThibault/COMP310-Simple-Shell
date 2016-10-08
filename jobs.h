#include "commands.h"

#define MAX_JOBS 10

int add_job(struct cmd_simple cmd);
void print_jobs();
void remove_job_by_cmd(struct cmd_simple *cmd);
void remove_job_by_index(int index);
void foreground_job(int index);
