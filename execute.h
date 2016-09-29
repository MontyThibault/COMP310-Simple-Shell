#include "commands.h"

void execute_generic(struct cmd_tagged_union *tagged_union);

void execute_cmd_simple(struct cmd_simple *cmd);
void execute_cmd_pipe(struct cmd_compound *cmd);
void execute_cmd_redirect(struct cmd_compound *cmd);
