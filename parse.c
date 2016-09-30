#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "parse.h"

//
// This code is given for illustration purposes. You need not include or follow this
// strictly. Feel free to writer better or bug free code. This example code block does not
// worry about deallocating memory. You need to ensure memory is allocated and deallocated
// properly so that your shell works without leaking memory.
//

int getcmd(char *prompt, char *args[] /*, int *background */) {

	// memset(args, 0, 20);

	int length, i = 0;
	char *token, *loc;
	char *line = NULL;
	size_t linecap = 0;

	printf("%s", prompt);
	length = getline(&line, &linecap, stdin);

	if (length <= 0) {
		exit(-1);
	}

	// Check if background is specified..
//	if ((loc = index(line, '&')) != NULL) {
//		*background = 1;
//		*loc = ' ';
//	} else {
//		*background = 0;
//	}

	while ((token = strsep(&line, " \t\n")) != NULL) {
		for (int j = 0; j < strlen(token); j++) {
			if (token[j] <= 32)
				token[j] = '\0';
		}

		if (strlen(token) > 0) {
			args[i++] = token;
		}
	}

	args[i] = "\0";

	return i;
}

