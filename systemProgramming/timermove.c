#include <stdio.h>
#include <string.h>
#include <termio.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>

#define EXT_KEY       91   //확장키 인식값 
#define KEY_LEFT      68
#define KEY_RIGHT     67
#define KEY_UP        65
#define KEY_DOWN      66
#define INTERVAL 1 // 단위 초 

void printMap(int map[10][10]) {
   for (int i = 0; i < 10; i++) {
      for (int j = 0; j < 10; j++) {
         switch (map[i][j]) {
         case 0:
            printf("□ ");
            break;
         case 1:
            printf("■ ");
            break;
         case 2:
            printf("1 ");
            break;
         case 3:
            printf("1 ");
            break;
         case 4:
            printf("2 ");
            break;
         case 5:
            printf("2 ");
            break;
         case 6:
            printf("★ ");
         default:
            break;
         }
      }
      printf("\n");
   }

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

void mytimer(int signo)
	{
		printf("제한시간 초과");
		alarm(INTERVAL); //알람처리후 다시 등록=반복
	}



int main() {

	struct sigaction act;
	act.sa_handler = mytimer;
	
   int map[10][10] = { 
   {5,0,0,1,0,0,0,1,0,3}, //0
   {1,1,0,0,1,0,0,1,0,0}, //1
   {0,0,0,0,0,1,0,1,0,0}, //2
   {0,0,1,1,1,0,0,0,0,0}, //3
   {0,0,0,0,0,1,0,0,1,0}, //4
   {0,0,1,0,0,1,0,1,0,0}, //5
   {1,0,0,0,0,1,0,0,1,0}, //6
   {0,0,0,1,0,0,0,0,0,0}, //7
   {0,1,0,1,1,1,1,1,1,0}, //8
   {2,0,0,0,0,1,0,0,0,4}, //9
   };
   //플레이어 시작 위치, 도착 위치는 2 3 4 5로 설정해놓음.
   //6은 플레이어1, 7은 플레이어2 움직일 말
   //벽이 1이고 갈수있는곳이 0임
   
   printMap(map);

   
   char keytemp;

   int playerX = 9;
   int playerY = 9;
   for (; ;)
   {
      if (kbhit())
      {
         system("clear");
         keytemp = getch();
         if (keytemp == EXT_KEY)
         {
            keytemp = getch();
            switch (keytemp)
            {
            
            case KEY_UP:      //상
               if (playerY >= 1) {
                  if (map[playerY-1][playerX] == 1) {
                     playerX = 9;
                     playerY = 9;
                  }
                  else {
                     playerY--;
                  }
               }
                  
                  
               break;
            case KEY_DOWN:      //하
               if (playerY <= 8) {
                  if (map[playerY+1][playerX] == 1) {
                     playerX = 9;
                     playerY = 9;
                  }
                  else {
                     playerY++;
                  }
               }
               break;
            case KEY_LEFT:      //좌
               if (playerX >= 1) {
                  if (map[playerY][playerX-1] == 1) {
                     playerX = 9;
                     playerY = 9;
                  }
                  else {
                     playerX--;
                  }
               }   
               break;
            case KEY_RIGHT:      //우
               if (playerX <= 8) {
                  if (map[playerY][playerX+1] == 1) {
                     playerX = 9;
                     playerY = 9;
                  }
                  else {
                     playerX++;
                  }
               }
               break;
            }
         }
         map[playerY][playerX] = 6;
         printMap(map);
         map[playerY][playerX] = 0; 
      }
      
   }
sigemptyset(&act.sa_mask);
act.sa_flags=0;

sigaction(SIGALRM, &act, 0);
alarm(INTERVAL);

	while(1)
	{
		sleep(100);
	}	

   return 0;
}
