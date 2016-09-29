#include <stdio.h>
#include <string.h>
#include "minunit.h"

#include "commands.h"
#include "history.h"
#include "automaton.h"


int tests_run = 0;


char *test_create_command() {
	
	char *argv[] = { "hello", "world" };
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

char *test_automaton() {
	return 0;
}

char *all_tests() {
	mu_run_test(test_create_command);
	mu_run_test(test_history);
	mu_run_test(test_automaton);

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
