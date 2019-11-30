#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int get_nick();

int main(){
	get_nick();
	return 0;
}

int get_nick(){
	char nick1[20];
	char nick2[20];
	int pipe_fd[2];
	char buff[100];
	pid_t pid;
	int status;
	
	if(-1 == pipe(pipe_fd)){
		perror("파이프 생성 실패");
		exit(1);
	}

	printf("플레이어1의 닉네임 : ");
	scanf("%s",nick1);

	if((pid = fork()) == 0){ //자식 프로세스
		while(1){
			printf("플레이어2의 닉네임 : ");
			scanf("%s", nick2);
			if(!strcmp(nick1,nick2)){
				printf("닉네임이 중복되었습니다. 플레이어2의 닉네임을 다시 입력해주세요.\n");
			}
			else
				break;
		}
		write(pipe_fd[1],nick2,strlen(nick2)); //중복이 아닐 시 부모로 데이터 전송
		printf("파이프 전송\n");
		exit(1);
		
	}
	else { //부모 프로세스
		pid = wait(&status);
		// 자식 프로세스 종료 대기
		if(status){
			memset(nick2,0,20);
			read(pipe_fd[0],nick2,20); //자식으로부터 닉네임을 받아옴
			printf("플레이어1의 닉네임 : %s, 플레이어 2의 닉네임 : %s\n",nick1, nick2);
			}
		else{
			printf("뭔가 오류임\n");
		}
		
	}
	return 0;
	
}
