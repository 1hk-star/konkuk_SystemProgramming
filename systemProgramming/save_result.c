#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
//#include <unistd.h>
#include <sys/stat.h>

#define MAX 1024

void save_result(char* win_player, char* lose_player, time_t whole_game_time) {
	int fd, n;
	char buf1[30] = { 0 };
	char buf2[20] = { 0 };
	char buf3[100] = { 0 };
	int temp= whole_game_time;
	char buf4[100] = { 0 };
	sprintf(buf4, "%d", temp);
	strcat(buf4, "초 걸림,  ");
	char buf5[2] = "\n";
	time(&whole_game_time);
	strcpy(buf1, win_player);
	strcat(buf1, "승리, ");
	strcpy(buf2, lose_player);
	strcat(buf2, "패배    ");
	strcpy(buf3, ctime(&whole_game_time));
	strcat(buf1, buf2);
	fd = open("./result.txt", O_RDWR | O_APPEND);
	if (fd < 0) {
		printf("\nError opening file");
		exit(0);
	}
	n = write(fd, buf1, 30);
	n = write(fd, buf4, 100);
	n = write(fd, buf3, 100);
	//n = write(fd, buf5, 2); //공백용인데 buf3을 write할때 ctime하면 자동으로 \n들어감
	close(fd);
}
int main() {
	time_t seconds=201616;
	save_result("333", "kim", seconds);
	return 0;
}

