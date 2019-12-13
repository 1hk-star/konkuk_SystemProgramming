#include <errno.h>
#include <termio.h>
#include <termios.h>
#include <signal.h>
#include <sys/time.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>

#define ROWS 10
#define COLS 10

#define KEY_LEFT1      97
#define KEY_RIGHT1     100
#define KEY_UP1        119
#define KEY_DOWN1      115

#define KEY_LEFT2      106
#define KEY_RIGHT2     108
#define KEY_UP2        105
#define KEY_DOWN2      107

#define ROAD            0
#define WALL            1
#define START_1         2
#define DEST_1          3
#define START_2         4
#define DEST_2          5
#define CUR_POSITION_1  6
#define CUR_POSITION_2  7
#define MAX_TIME        30

int map[ROWS][COLS]; // 일단 전역으로 만들고 나중에 멤버로 넣든지 하기
int timeCount = MAX_TIME;

typedef struct player{
   int x;
   int y;
   char nickname[20];
} Player;

void gotoxy(int x,int y);
int kbhit(void);
int getch(void);
int readline(int fd, int *buf, int nbytes);

void show_main_menu();
void show_result(); 
void how_to_play();

void timer(int signo); // timer 핸들러 함수
int createTimer(timer_t* timerID, int sec, int msec);

int select_map();
void printMap(int map[ROWS][COLS]);
void showStatus(const int flag, const char* player1_nick, const char* player2_nick);

int game_play();
int get_nick(Player* player1, Player* player2);

void init(int map[ROWS][COLS], Player* player1, Player* player2);


int main(){
   show_main_menu();

   return 0;
}

void show_main_menu(){
    printf("\033c");
	int a;
    while(1){
        printf("1. 게임시작\n2. 최근 10경기 결과보기\n3. 게임방법\n4. 게임종료\n");
        printf("메뉴를 선택하세요 : ");
        scanf("%d", &a);
        system("clear");
        if(a == 1){ // 게임 시작
            game_play();
            // get_nick();
        }
        else if(a == 2){ // 최근 10경기 결과 보기
            show_result();
        }
        else if(a == 3){ // 게임 방법 설명
            how_to_play();
        }
        else if(a == 4){
            printf("게임 종료\n"); 

            return; // 종료
        }
        else{
            printf("1 ~ 3사이의 정수를 입력하세요.\n");
            continue;
        }

    }
}

void clean_stdin(void){
	int c;
	do{
		c = getchar();
	} while(c!='\n' && c != EOF);
}

void how_to_play(){ // 게임 설명 추가하기.
	clean_stdin();	
	printf("\033c");	//화면 clear
	printf("<기억의 미로 게임 방법>\n");
	sleep(1);
	printf("기억의 미로는 2인이 동시에 즐길 수 있는 게임입니다.");
	printf("\n게임 방법은 다음과 같습니다. (아무 키나 입력하세요)\n");
	getch();	

	printf("\n게임 시작 전 플레이어의 닉네임을 입력받습니다.\n");
	printf("이때, 두 닉네임은 동일해서는 안됩니다.(아무 키나 입력하세요)\n");
	getch();

	printf("\n게임이 시작되면 플레이어는 30초 동안 세 칸을 움직여야 합니다.\n");
	printf("맵 곳곳에는 미로와 같이 벽들이 존재할 수 있으며,\n");
	printf("이 벽에 플레이어가 닿을 시, 남은 이동 횟수와 관계 없이 처음으로 돌아가고 다른 플레이어에게 차례가 넘어갑니다.\n");
	printf("즉, 플레이어가 세 칸을 움직이거나, 30초가 경과하거나, 벽에 부딪히면 턴이 넘어갑니다.(아무 키나 입력하세요)\n");
	printf("왼쪽 플레이어의 방향키는 WASD입니다.\n");
	printf("오른쪽 플레이어의 방향키는 IJKL입니다.\n");
	getch();

	printf("\n각 플레이어의 목적지에 가장 먼저 도착하는 플레이어가 승리를 하게 됩니다.\n");
	printf("게임 설명은 여기서 끝입니다.(아무 키나 입력하세요)\n");
	getch();
	
	printf("\n보이지 위협이 도사리는 곳에서 당신의 직감을 시험해보시길\n3초 뒤에 메뉴가 뜹니다.\n");
	sleep(3);	
}

int game_play(){
   Player player1;
   Player player2;

   get_nick(&player1, &player2);

   if(select_map(map) == -1){
      perror("Fail To Seelct Maze!!");
      return -1;
   }
   init(map, &player1, &player2); // 게임 정보 초기화 

   char keytemp;

   int playerFlag = 1; // 1이면 player1차례, 2이면 player2차례
   int count = 0;
   int preValue = -1;

   const int WIN_FLAG1 = 3;
   const int WIN_FLAG2 = 4;

   showStatus(playerFlag, "Player1", "Player2");
   printMap(map);

   timer_t timerID;
   if(createTimer(&timerID, 1, 0) == -1){
      perror("Failed to Create Timer!!");
      return -1;
   }

   time_t startTime;
   if( (startTime = time(NULL)) == -1){
      perror("Failed To Call time!!");
      return -1;
   }

   for (; ;)
   {
      if(timeCount == 0){
         playerFlag = playerFlag % 2 + 1;
         timeCount = MAX_TIME;
         continue;
      }

      if(playerFlag == 1){

         keytemp = getch();
         switch (keytemp)
         {
                  
         case KEY_UP1:      //상
            if (player1.y >= 1) {
               if (map[player1.y-1][player1.x] == WALL) {
                  player1.y = ROWS - 1;
                  player1.x = 0;
                  playerFlag = playerFlag % 2 + 1;
               }
               else if(map[player1.y-1][player1.x] == DEST_1){
                  playerFlag = WIN_FLAG1;
               }
               else {
                  player1.y--;
                  count++;
               }
            }
                     
            break;
         case KEY_DOWN1:      //하
            if (player1.y <= 8) {
               if (map[player1.y+1][player1.x] == WALL) {
                  player1.y = ROWS - 1;
                  player1.x = 0;
                  playerFlag = playerFlag % 2 + 1;
               }
               else if(map[player1.y+1][player1.x] == DEST_1){
                  playerFlag = WIN_FLAG1; 
               }
               else {
                  player1.y++;
                  count++;
               }
            }

            break;
            case KEY_LEFT1:      //좌
               if (player1.x >= 1) {
                  if (map[player1.y][player1.x-1] == WALL) {
                     player1.y = ROWS - 1;
                     player1.x = 0;
                     playerFlag = playerFlag % 2 + 1;
                           
                  }
                  else if(map[player1.y][player1.x-1] == DEST_1){
                     playerFlag = WIN_FLAG1;
                  }
                  else {
                     player1.x--;
                     count++;
                  }
               }

               break;
            case KEY_RIGHT1:      //우
               if (player1.x <= 8) {
                  if (map[player1.y][player1.x + 1] == WALL) {
                        player1.y = COLS - 1;
                        player1.x = 0;
                        playerFlag = playerFlag % 2 + 1;

                  }
                  else if(map[player1.y][player1.x + 1] == DEST_1){
                     playerFlag = WIN_FLAG1;  
                  }
                  else {
                     player1.x++;
                     count++;
                  }
               }

               break;
         }
         preValue = map[player1.y][player1.x];
         system("clear");
         map[player1.y][player1.x] = CUR_POSITION_1;
         showStatus(playerFlag, "Player1", "Player2");
         printMap(map);
         
         map[player1.y][player1.x] = preValue;
      }
      
      else if(playerFlag == 2){

         keytemp = getch();
         switch (keytemp)
         {
                  
         case KEY_UP2:      //상
            if (player2.y >= 1) {
               if (map[player2.y-1][player2.x] == WALL) {
                  player2.y = ROWS - 1;
                  player2.x = COLS - 1;
                  playerFlag = playerFlag % 2 + 1;
                           
               }
               else if(map[player2.y-1][player2.x] == DEST_2){
                  playerFlag = WIN_FLAG2;  
               }
               else {
                  player2.y--;
                  count++;
               }
            }
                     
            break;
         case KEY_DOWN2:      //하
            if (player2.y <= 8) {
               if (map[player2.y+1][player2.x] == WALL) {
                  player2.y = ROWS - 1;
                  player2.x = COLS - 1;
                  playerFlag = playerFlag % 2 + 1;

               }
               else if(map[player2.y+1][player2.x] == DEST_2){
                  playerFlag = WIN_FLAG2;
               }
               else {
                  player2.y++;
                  count++;
               }
            }

               break;
            case KEY_LEFT2:      //좌
               if (player2.x >= 1) {
                  if (map[player2.y][player2.x-1] == WALL) {
                     player2.y = ROWS - 1;
                     player2.x = COLS - 1;
                     playerFlag = playerFlag % 2 + 1;
                  }
                  else if(map[player2.y][player2.x-1] == DEST_2){
                     playerFlag = WIN_FLAG2;
                  }  
                  else {
                     player2.x--;
                     count++;
                  }
               }

               break;
            case KEY_RIGHT2:      //우
               if (player2.x <= 8) {
                  if (map[player2.y][player2.x + 1] == WALL) {
                        player2.y = ROWS - 1;
                        player2.x = COLS - 1;
                        playerFlag = playerFlag % 2 + 1;
                  }
                  else if(map[player2.y][player2.x + 1] == DEST_2){
                     playerFlag = WIN_FLAG2;
                  } 
                  else {
                     player2.x++;
                     count++;
                  }
               }

               break;
         }
         preValue = map[player2.y][player2.x];
         system("clear");
         map[player2.y][player2.x] = CUR_POSITION_2;
         showStatus(playerFlag, "Player1", "Player2");
         printMap(map);
         
         map[player2.y][player2.x] = preValue;
      }

      if(playerFlag == WIN_FLAG1){
         time_t endTime;
         if( (endTime = time(NULL)) == -1){
            perror("Failed To Call time!!");
            return -1;
         }

         double entireTime = difftime(endTime, startTime); // 전체 플레이 시간

         system("clear");
         printf("■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n");
         printf("■                       ■\n");
         printf("■                       ■\n");
         printf("■    %s승리!!■", player1.nickname);
         printf("■                       ■\n");
         printf("■                       ■\n");
         printf("■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n");
         timer_delete(timerID);

         sleep(5);
         system("clear");

         return 0;
      }
      else if(playerFlag == WIN_FLAG2){
         time_t endTime;
         if( (endTime = time(NULL)) == -1){
            perror("Failed To Call time!!");
            return -1;
         }

         double entireTime = difftime(startTime, endTime);
         
         system("clear");
         printf("■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n");
         printf("■                       ■\n");
         printf("■                       ■\n");
         printf("■    %s승리!!■", player2.nickname);
         printf("■                       ■\n");
         printf("■                       ■\n");
         printf("■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n");
         timer_delete(timerID);

         sleep(5);
         system("clear");

         return 0;
      }

      if(count == 3){
         count = 0;
         playerFlag = playerFlag % 2 + 1;
         timeCount = MAX_TIME;
         gotoxy(0, 3);
         printf("턴이 종료됩니다.\n");
         sleep(1);
         // timer_delete(&timerID);
         // createTimer(&timerID, 5, 0);
      }

   }


   return 0;


}



int get_nick(Player* player1, Player* player2){
        //char nick1[20];
        //char nick2[20];
        int pipe_fd[2];
        char buff[100];
        pid_t pid;
        int status;

        if(-1 == pipe(pipe_fd)){
            perror("파이프 생성 실패");
            exit(1);
        }

        printf("플레이어1의 닉네임 : ");
        scanf("%s",player1->nickname);

        if((pid = fork()) == 0){ //자식 프로세스
                while(1){
                        printf("플레이어2의 닉네임 : ");
                        scanf("%s", player2->nickname);
                        if(!strcmp(player1->nickname,player2->nickname)){
                                printf("닉네임이 중복되었습니다. 플레이어2의 닉네임을 다시 입력해주세요.\n");
                        }
                        else
                                break;
                }
                write(pipe_fd[1],player2->nickname,strlen(player2->nickname)); //중복이 아닐 시 부모로 데이터 전송
                printf("파이프 전송\n");
                exit(1);

        }
        else { //부모 프로세스
                pid = wait(&status);
                // 자식 프로세스 종료 대기
                if(status){
                        memset(player2->nickname,0,20);
                        read(pipe_fd[0],player2->nickname,20); //자식으로부터 닉네임을 받아옴
                        printf("플레이어1의 닉네임 : %s, 플레이어 2의 닉네임 : %s\n",player1->nickname, player2->nickname);
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

void gotoxy(int x,int y)
{
    printf("%c[%d;%df",0x1B,y,x);
}

void timer(int signo)
{
    gotoxy(0, 2);

    if(timeCount > 0){
        timeCount = timeCount - 1;
        printf("남은 시간 : %d초", timeCount);
    }

}

int createTimer(timer_t* timerID, int sec, int msec){
   struct sigevent    te;
   struct itimerspec   its;
   struct sigaction    sa;
   int signo = SIGALRM;

   // sa.sa_flags = SA_SIGINFO;
   //sa.sa_sigaction = timer;
   sa.sa_handler = timer;
   sa.sa_flags = 0;
   if(sigemptyset(&sa.sa_mask) == -1 || sigaction(signo, &sa, NULL) == -1){
      perror("Fail to register signal handler!!");
      return -1;
   }

   // te.sigev_notify = SIGEV_SIGNAL;  
   // te.sigev_signo = signo;  
   // te.sigev_value.sival_ptr = timerID;  

   // timer_create(CLOCK_REALTIME, &te, timerID);  
   if(timer_create(CLOCK_REALTIME, NULL, timerID) == -1){
      perror("Failed to Create Timer!!");
      return -1;
   }

   its.it_interval.tv_sec = sec;
   // its.it_interval.tv_nsec = msec * 1000000;  
   its.it_interval.tv_nsec = 0;
   its.it_value.tv_sec = sec;

   // its.it_value.tv_nsec = msec * 1000000;
   its.it_value.tv_nsec = 0;

   return timer_settime(*timerID, 0, &its, NULL);
}

int kbhit(void)
{
   struct termios oldt, newt;
   int ch;
   tcgetattr(STDIN_FILENO, &oldt);
   newt = oldt;
   newt.c_lflag &= ~(ICANON | ECHO);
   tcsetattr(STDIN_FILENO, TCSANOW, &newt);
   ch = getchar();
   tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
   return ch;
}

int getch(void) {
   int ch;
   struct termios old;
   struct termios new;
   tcgetattr(0, &old);
   new = old;
   new.c_lflag &= ~(ICANON|ECHO);
   new.c_cc[VMIN] = 1;
   new.c_cc[VTIME] = 0;
   tcsetattr(0, TCSAFLUSH, &new);
   ch = getchar();
   tcsetattr(0, TCSAFLUSH, &old);
   return ch;
}

int readline(int fd, int *buf, int nbytes) {
   int numread = 0;
   int returnval;

   while (numread < nbytes + 2) {
      returnval = read(fd, buf + numread, 1);
      if ((returnval == -1) && (errno == EINTR))
         continue;
      if ( (returnval == 0) && (numread == 0) )
         return 0;
      if (returnval == 0)
         break;
      if (returnval == -1)
         return -1;
      numread++;

      // '\n'을 13(Carriage Return)과 10(Line Feed) 2개로 읽어서 이를 처리.
      if (buf[numread-1] == 13) {
        continue;
      }
      else if(buf[numread-1] == 10){
          return (numread - 2); // 맞는지 모르겠지만 13(Carriage Return)과 10(Line Feed)제외한 숫자를 읽은 개수 return.
      }

   }
   errno = EINVAL;
   return -1;
}

void printMap(int map[ROWS][COLS]) {
   gotoxy(0, 4);
   for (int i = 0; i < ROWS; i++) {
      for (int j = 0; j < COLS; j++) {
         switch (map[i][j]) {
         case ROAD:
            printf("□ "); // 갈 수 있는 길
            break;
         case WALL:
            printf("■ "); // 벽
            break;
         case START_1:
            printf("1 "); // 플레이어 1의 시작점
            break;
         case DEST_1:
            printf("1 "); // 플레이어 1의 도착점
            break;
         case START_2:
            printf("2 "); // 플레이어 2의 시작점
            break;
         case DEST_2:
            printf("2 "); // 플레이어 2의 도착점
            break;
         case CUR_POSITION_1:
            printf("★ "); // 플레이어 1의 현재 위치
            break;
         case CUR_POSITION_2:
            printf("♥ "); // 플레이어 2의 현재 위치
            break;
         default:
            break;
         }
      }
      printf("\n");
   }
}

void showStatus(const int flag, const char* player1_nick, const char* player2_nick){
   gotoxy(0, 0);

   if(flag == 1){
      printf("< %s의 차례 >\n", player1_nick);
   }
   else{
      printf("< %s의 차례 >\n", player2_nick);
   }
}

int select_map(){
    srand((unsigned)time(NULL));

    int mapindex = (rand() % 2) + 1; // map1.txt과 map2.txt 중 랜덤으로 하나 고르기 위함.
    int mapfd;

    if(mapindex == 1){
        if( (mapfd = open("map1.txt", O_RDONLY)) == -1){
            perror("Failed to open map file");
            return -1;
        }

    }
    else if(mapindex == 2){
        if( (mapfd = open("map2.txt", O_RDONLY)) == -1){
            perror("Failed to open map file");
            return -1;
        }
    }

    for(int i = 0; i < ROWS; i++){
        readline(mapfd, map[i], 10); // 한 줄식 ROWS번 읽음 ==> 10x10을 읽게 된다.

        for(int j = 0; j < COLS; j++){
            map[i][j] -= '0';
        }
    }

    // // maze에 잘 저장되었나 출력을 통해 test하기 위한 구문
    // for(int i = 0; i < ROWS; i++){
    //     for(int j = 0; j < COLS; j++){
    //         printf("%d ", maze[i][j]);
    //     }
    //     printf("\n");
    // }

    return 0;
}

void init(int map[ROWS][COLS], Player* player1, Player* player2){
   player1->y = ROWS - 1;
   player1->x = 0;
   player2->y = ROWS - 1;
   player2->x = COLS - 1;
   
   map[player1->y][player1->x] = CUR_POSITION_1;
   map[player2->y][player2->x] = CUR_POSITION_2;
   map[0][0] = DEST_2;
   map[ROWS - 1][0] = START_1;
   map[ROWS - 1][COLS - 1] = START_2;
   map[0][COLS - 1] = DEST_1;
}

