#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#define INTERVAL 1 // 단위가 초(sec)

void mytimer(int signo)
	{
printf("타이머 부르기..\n");
alarm(INTERVAL);
//알람처리후 다시 알람 등록되어 계속 구동
	}

int main()
{
struct sigaction act;
act.sa_handler=mytimer;
//SIGALRM 이란 시그널 발생할 경우 동작시킬 핸들러 함수
//그게 발생하면 mytimer 호출 
sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	sigaction(SIGALRM, &act, 0);
		alarm(INTERVAL);

	while(1)

	{
	sleep(100);
	}
	}


