#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>


void show_result() {
	pid_t pid;
	int status;
	if ((pid = fork()) == 0) { //자식 프로세스
		execl("/usr/bin/tail", "tail", "-n10", "./result.txt", NULL);
	}
	else { //부모 프로세스
		pid = wait(&status);
		// 자식 프로세스 종료 대기
	}
}

int main() {
	show_result();
	return 0;
}

