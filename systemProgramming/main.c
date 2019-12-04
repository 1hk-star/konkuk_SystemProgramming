
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void show_main_menu();
void game_play();
int get_nick();
void show_result();

int main(){
	show_main_menu();
	//get_nick();
	return 0;
}

void show_main_menu(){
    int a;
    while(1){
	printf("1. 게임시작\n 2. 최근 10경기 결과보기\n 3. 게임종료\n");
	scanf("%d", &a);	
        if(a == 1){
            game_play();
            // get_nick();
	}
        else if(a == 2){
	    printf("2번 메뉴\n");
            show_result();
	}
        else if(a == 3){
	    printf("3번 메뉴\n");
            return;
	}
        else{
            printf("1 ~ 3사이의 정수를 입력하세요.\n");
            continue; 
        }
       
    }

}

void game_play(){
    get_nick();

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
