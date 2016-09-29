#define ARG_ARRAY_LENGTH 20
#define ARG_BUFFER_SIZE 20


#ifndef INCLUDE_COMMAND
#define INCLUDE_COMMAND

struct cmd_simple {
	char args[ARG_ARRAY_LENGTH][ARG_BUFFER_SIZE];
	int len;
};

struct cmd_compound {
	struct cmd_simple cmd1;
	struct cmd_simple cmd2;
};

enum cmd_type {
	t_simple,
	t_pipe,
	t_redirect
};

union cmd_union {
	struct cmd_simple u_simple;
	struct cmd_compound u_compound;
};

struct cmd_tagged_union {
	enum cmd_type type;
	union cmd_union value;
};


struct cmd_simple *cmd_extract_fst(struct cmd_compound *cmd);
struct cmd_simple *cmd_extract_snd(struct cmd_compound *cmd);

char *cmd_extract_arg_at(struct cmd_simple *cmd, int index);
char *cmd_extract_program(struct cmd_simple *cmd);

void cmd_copy_from_ptr_arr(struct cmd_simple *cmd, char *arr[], int len);
void cmd_generate_ptr_arr(struct cmd_simple *cmd, char *arr[]);

#endif
