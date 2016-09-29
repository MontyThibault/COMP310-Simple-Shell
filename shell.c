#include "history.h"
#include "parse.h"


int main(void) {

	char *args[20];
	int bg;
	while(1) {

		bg = 0;
		int cnt = getcmd("\n>> ", args, &bg);

		// bg - whether a command is background process or not
		// cnt - the number of arguments in args[]
	

		// Terminate the arguments array
		args[cnt] = '\0';

		
		hist_add(args, cnt);
		if(cnt > 5) {
			struct history_entry *e = hist_fetch(1);		
			hist_generate_ptr_arr(e, args);
		}		
	
		
		// Print arguments (debug)

		// Up next - a better tokenizer!

		for(int i = 0; args[i] != '\0'; i++) {
			// printf("%s\n", args[i]);
		}		


		int pid = fork();
	
		if(pid == 0) {
			execvp(args[0], args);
			printf("There was an error.");
		}

		if(bg == 0) {
			
			int status;
			waitpid(pid, &status, 0);
		}

 /* the steps can be..:
(1) fork a child process using fork()
(2) the child process will invoke execvp()
(3) if background is not specified, the parent will wait,
otherwise parent starts the next command... */
	}
}
