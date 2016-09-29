#include "commands.h"

#define HIST_SIZE 10

void hist_add(struct cmd_tagged_union cmd);
struct cmd_tagged_union *hist_fetch(int cmd_num);
