#include <stdio.h>
#include <string.h>
#include "minunit.h"

#include "commands.h"
#include "history.h"
#include "automaton.h"
#include "match.h"


int tests_run = 0;


char *test_create_command() {
	
	char *argv[] = { "hello", "world", "\0" };
	int argc = 2;
	
	struct cmd_tagged_union cmd_tagged_union;
	cmd_tagged_union.type = t_simple;

	struct cmd_simple cmd = cmd_tagged_union.value.u_simple;
	cmd_copy_from_ptr_arr(&cmd, argv, argc);

	return 0;
}

char *test_history() {

	struct cmd_tagged_union tagged_union;
	tagged_union.value.u_simple.len = 123;

	for(int i = 0; i < 100; i++) {
		hist_add(tagged_union);
	}

	struct cmd_tagged_union *entry = hist_fetch(100);

	int entry_len = (*entry).value.u_simple.len;
	mu_assert("History entries do not match", entry_len == 123);

	return 0;
}

char *test_match_simple() {

	char *argv[] = { "gcc", "-o", "myfile.c", "\0" };
	int argc = 3;

	struct automaton_state state = automaton_new(argv);

	state = match_full_simple(state);

	mu_assert("Simple command did not match", state.acceptance_state == t_accepting);
	mu_assert("First argument does not match",
		strcmp(cmd_extract_arg_at(&state.cmd.value.u_simple, 0), argv[0]) == 0);
	mu_assert("Args length do not match", state.cmd.value.u_simple.len == argc);

	return 0;
}

char *test_match_compound() {

	char *argv[] = { "gcc", "-o", "myfile.c", "|", "hello", "world", "\0" };
	int argc = 3;

	struct automaton_state state = automaton_new(argv);

	state = match_full_pipe(state);

	mu_assert("Pipe command did not match", state.acceptance_state == t_accepting);
	mu_assert("First argument of first command does not match",
		strcmp(cmd_extract_arg_at(&state.cmd.value.u_compound.cmd1, 0), "gcc") == 0);
	mu_assert("First argument of second command does not match",
		strcmp(cmd_extract_arg_at(&state.cmd.value.u_compound.cmd2, 0), "hello") == 0);
	mu_assert("cmd2 args length do not match", state.cmd.value.u_compound.cmd2.len == 2);

	return 0;
}

char *all_tests() {
	mu_run_test(test_create_command);
	mu_run_test(test_history);
	mu_run_test(test_match_simple);
	mu_run_test(test_match_compound);

	return 0;
}

int main(int argc, char *argv[]) {

	char *result = all_tests();
	if(result != 0) {
		printf("%s\n", result);
	} else {
		printf("ALL TESTS PASSED\n");
	}

	printf("Tests run: %d\n", tests_run);

	return result != 0;	
}
