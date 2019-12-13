#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termio.h>
#include <termios.h>
#include <signal.h>
#include <sys/time.h>


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
} Player;

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


int main() {


   /* ************ 추가로 해야 될 점 *******************
   지금 map이 전역변수로 선언되어 있는데 이것을 지역 변수로 만들고
   select_map(map)과 같이 select_map함수를 이용해서 초기화 시키기
   */

    timer_t timerID;

    createTimer(&timerID, 1, 0);

   if(select_map(map) == -1){
      perror("Fail To Seelct Maze!!");
      return -1;
   }
   int playerFlag = 1; // 1이면 player1이면, 2이면 player2

   // int map[ROWS][COLS] = { 
   // {5,0,0,1,0,0,0,1,0,3}, //0
   // {1,1,0,0,1,0,0,1,0,0}, //1
   // {0,0,0,0,0,1,0,1,0,0}, //2
   // {0,0,1,1,1,0,0,0,0,0}, //3
   // {0,0,0,0,0,1,0,0,1,0}, //4
   // {0,0,1,0,0,1,0,1,0,0}, //5
   // {1,0,0,0,0,1,0,0,1,0}, //6
   // {0,0,0,1,0,0,0,0,0,0}, //7
   // {0,1,0,1,1,1,1,1,1,0}, //8
   // {2,0,0,0,0,1,0,0,0,4}, //9
   // };
   //플레이어 시작 위치, 도착 위치는 2 3 4 5로 설정해놓음.
   //6은 플레이어1, 7은 플레이어2 움직일 말
   //벽이 1이고 갈수있는곳이 0임

   Player player1;
   Player player2;

   // 밑에 정보들을 init()함수 하나 만들어서 해도 될듯.
   player1.y = ROWS - 1;
   player1.x = 0;
   player2.y = ROWS - 1;
   player2.x = COLS - 1;
   
   map[player1.y][player1.x] = CUR_POSITION_1;
   map[player2.y][player2.x] = CUR_POSITION_2;
   map[0][0] = DEST_2;
   map[ROWS - 1][0] = START_1;
   map[ROWS - 1][COLS - 1] = START_2;
   map[0][COLS - 1] = DEST_1;

   showStatus(playerFlag, "Player1", "Player2");
   printMap(map);
   
   char keytemp;
   int count = 0;
   int preValue = -1;
   for (; ;)
   {
      printf("timeCount : %d", timeCount);
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
                     playerFlag = 3;
                     //gotoxy(50, 10);
                     printf("■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n");
                     printf("■                       ■\n");
                     printf("■                       ■\n");
                     printf("■    %s승리!!■", "player1");
                     printf("■                       ■\n");
                     printf("■                       ■\n");
                     printf("■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n");
                     
                     return 0;
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
                     playerFlag = 3;
                     //gotoxy(50, 10);
                     printf("■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n");
                     printf("■                       ■\n");
                     printf("■                       ■\n");
                     printf("■    %s승리!!■", "player1");
                     printf("■                       ■\n");
                     printf("■                       ■\n");
                     printf("■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n");

                     return 0;
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
                     playerFlag = 3;
                     //gotoxy(50, 10);
                     printf("■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n");
                     printf("■                       ■\n");
                     printf("■                       ■\n");
                     printf("■    %s승리!!■", "player1");
                     printf("■                       ■\n");
                     printf("■                       ■\n");
                     printf("■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n");

                     return 0;
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
                     playerFlag = 3;
                     //gotoxy(50, 10);
                     printf("■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n");
                     printf("■                       ■\n");
                     printf("■                       ■\n");
                     printf("■    %s승리!!■", "player1");
                     printf("■                       ■\n");
                     printf("■                       ■\n");
                     printf("■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n");

                     return 0;
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
                     playerFlag = 3;
                     //gotoxy(50, 10);
                     printf("■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n");
                     printf("■                       ■\n");
                     printf("■                       ■\n");
                     printf("■    %s승리!!■", "player2");
                     printf("■                       ■\n");
                     printf("■                       ■\n");
                     printf("■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n");

                     return 0;
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
                     playerFlag = 3;
                     //gotoxy(50, 10);
                     printf("■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n");
                     printf("■                       ■\n");
                     printf("■                       ■\n");
                     printf("■    %s승리!!■", "player2");
                     printf("■                       ■\n");
                     printf("■                       ■\n");
                     printf("■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n");

                     return 0;
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
                     playerFlag = 3;
                     //gotoxy(50, 10);
                     printf("■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n");
                     printf("■                       ■\n");
                     printf("■                       ■\n");
                     printf("■    %s승리!!■", "player2");
                     printf("■                       ■\n");
                     printf("■                       ■\n");
                     printf("■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n");

                     return 0;
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
                     playerFlag = 3;
                     //gotoxy(50, 10);
                     printf("■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n");
                     printf("■                       ■\n");
                     printf("■                       ■\n");
                     printf("■    %s승리!!■", "player2");
                     printf("■                       ■\n");
                     printf("■                       ■\n");
                     printf("■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n");

                     return 0;
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

      if(count == 3){
         count = 0;
         playerFlag = playerFlag % 2 + 1;
         timeCount = MAX_TIME;
         gotoxy(0, 18);
         printf("턴이 종료됩니다.\n");
         sleep(1);
         // timer_delete(&timerID);
         // createTimer(&timerID, 5, 0);
      }

   }


   return 0;
}
