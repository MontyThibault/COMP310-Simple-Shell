#include <unistd.h>

// This program pipes stdin to stdout for testing
int main(int argc, char *argv[]) {

	int fd[2];
	pipe(fd);

	close(0); // Std input
	dup(fd[1]); // write end

	close(1); // std output
	dup(fd[0]); // read end

	while(1) {
		usleep(100000);
	}

	return 0;
}
