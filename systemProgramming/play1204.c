#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termio.h>
#include <termios.h>

#define ROWS 10
#define COLS 10

#define EXT_KEY        91   //확장키 인식값 
#define KEY_LEFT1      68
#define KEY_RIGHT1     67
#define KEY_UP1        65
#define KEY_DOWN1      66

#define KEY_LEFT2      97
#define KEY_RIGHT2     100
#define KEY_UP2        119
#define KEY_DOWN2      115

#define ROAD            0
#define WALL            1
#define START_1         2
#define DEST_1          3
#define START_2         4
#define DEST_2          5
#define CUR_POSITION_1  6
#define CUR_POSITION_2  7

int map[ROWS][COLS]; // 일단 전역으로 만들고 나중에 멤버로 넣든지 하기

typedef struct player{
   int x;
   int y;
} Player;

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
   player1.x = 9;
   player1.y = COLS - 1;
   player2.x = 0;
   player2.y = COLS - 1;

   map[player1.x][player1.y] = 9;
   map[0][0] = 3;
   map[0][COLS - 1] = 5;
   map[ROWS - 1][COLS - 1] = 2;
   map[ROWS - 1][0] = 4;

   printMap(map);

   // for(int i = 0; i < ROWS; i++){
   //    for(int j = 0; j < COLS; j++){
   //       printf("%d ", map[i][j]);
   //    }
   //    printf("\n");
   // }

   
   char keytemp;
   int count = 0;
 
   for (; ;)
   {
      
      if(playerFlag == 1){
         
         if (kbhit())
         {

            keytemp = getch();

            if(playerFlag == 1){
               //keytemp = getch();

               if (keytemp == EXT_KEY)
               {
                  
                  keytemp = getch();
                  switch (keytemp)
                  {
                  
                  case KEY_UP1:      //상
                     if (player1.y >= 1) {
                        if (map[player1.y-1][player1.x] == WALL) {
                           player1.x = COLS - 1;
                           player1.y = COLS - 1;
                           playerFlag = playerFlag % 2 + 1;
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
                           player1.x = COLS - 1;
                           player1.y = COLS - 1;
                           playerFlag = playerFlag % 2 + 1;
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
                           player1.x = COLS - 1;
                           player1.y = COLS - 1;
                           playerFlag = playerFlag % 2 + 1;
                        }
                        else {
                           player1.x--;
                           count++;
                        }
                     }   
                     break;
                  case KEY_RIGHT1:      //우
                     if (player1.x <= 8) {
                        if (map[player1.y][player1.x+1] == WALL) {
                           player1.x = COLS - 1;
                           player1.y = COLS - 1;
                           playerFlag = playerFlag % 2 + 1;
                        }
                        else {
                           player1.x++;
                           count++;
                        }
                     }

                     break;
                  }
                  system("clear");
                  map[player1.y][player1.x] = CUR_POSITION_1;
                  printMap(map);
                  map[player1.y][player1.x] = 0; 
               }
            }
            

            // if(count == 3){
            //    count = 0;
            //    playerFlag = playerFlag % 2 + 1;
            // }

            // map[player1_Y][player1_X] = CUR_POSITION_1;
            
            // printMap(map);
            // map[player1_Y][player1_X] = 0;

         }
      }
      else if(playerFlag == 2){
         
               
         // count++;
         // printf("keytemp : %d\n", keytemp);
         keytemp = getch();
         switch (keytemp)
         {
                  
         case KEY_UP2:      //상
            if (player2.y >= 1) {
               if (map[player2.y-1][player2.x] == WALL) {
                  player2.x = 0;
                  player2.y = COLS - 1;
                  playerFlag = playerFlag % 2 + 1;
                           
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
                  player2.x = 0;
                  player2.y = COLS - 1;
                  playerFlag = playerFlag % 2 + 1;

               }
               else {
                     player2.y++;
                     count++;
               }
            }

            break;
            case KEY_LEFT2:      //좌
               if (player2.x >= 1) {
                  if (map[player2.y][player2.y-1] == WALL) {
                     player2.x = 0;
                     player2.y = COLS - 1;
                     playerFlag = playerFlag % 2 + 1;
                           
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
                        player2.x = 0;
                        player2.y = COLS - 1;
                        playerFlag = playerFlag % 2 + 1;

                  }
                  else {
                        player2.x++;
                        count++;
                  }
               }

               break;
         }
      
         system("clear");
         map[player2.y][player2.x] = CUR_POSITION_2;
         printMap(map);
         map[player2.y][player2.x] = 0;
      }

      if(count == 3){
         count = 0;
         playerFlag = playerFlag % 2 + 1;
      }
   }


   return 0;
}
