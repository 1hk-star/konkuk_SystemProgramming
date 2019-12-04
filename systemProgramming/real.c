#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <stdio.h>
//타이머 주기에 따라 호출될 타이머
void timer()
{
	printf("timer\n");
}
int createTimer( timer_t *timerID, int sec, int msec)
{
	struct sigevent		 te;
	struct itimerspec	 its;
	struct sigaction       	sa;
	int sigNo=SIGRTMIN;
	//signal handler set up
	sa.sa_flags=SA_SIGINFO;
	sa.sa_sigaction=timer;
	sigemptyset(&sa.sa_mask);
	
	if(sigaction(sigNo, &sa, NULL)==-1)
	{
		printf("sigaction error\n");
		return -1;
	}

	//알람 설정 후 알람 작동
	te.sigev_notify=SIGEV_SIGNAL;
	te.sigev_signo=sigNo;
	te.sigev_value.sival_ptr=timerID;
	timer_create(CLOCK_REALTIME, &te, timerID);

	its.it_interval.tv_sec=sec;
	its.it_interval.tv_nsec=msec*1000000;
	its.it_value.tv_sec=sec;

	its.it_value.tv_nsec=msec*1000000;
	timer_settime(*timerID, 0, &its, NULL);

	return 0;
}
	int main()
{
	timer_t timerID;
 //타이머를 만든다
//매개변수 1 : 타이머 변수
//매개변수 2 : second
//매개변수 3:ms
	createTimer(&timerID, 5, 0);
	while(1)
	{
	}
}

