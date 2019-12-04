#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define ROWS 10
#define COLS 10

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


int** select_maze(void){
    srand((unsigned)time(NULL));

    int maze[ROWS][COLS] = {0};

    int mapindex = (rand() % 2) + 1; // map1.txt과 map2.txt 중 랜덤으로 하나 고르기 위함.
    int mapfd;

    if(mapindex == 1){
        if( (mapfd = open("map1.txt", O_RDONLY)) == -1){
            perror("Failed to open map file");
            return NULL;
        }   

    }
    else if(mapindex == 2){
        if( (mapfd = open("map2.txt", O_RDONLY)) == -1){
            perror("Failed to open map file");
            return NULL;
        }   

    }
    
    for(int i = 0; i < ROWS; i++){
        readline(mapfd, maze[i], 10); // 한 줄식 ROWS번 읽음 ==> 10x10을 읽게 된다.

        for(int j = 0; j < COLS; j++){
            maze[i][j] -= '0';
        }
    }

    // // maze에 잘 저장되었나 출력을 통해 test하기 위한 구문
    // for(int i = 0; i < ROWS; i++){
    //     for(int j = 0; j < COLS; j++){
    //         printf("%d ", maze[i][j]);
    //     }
    //     printf("\n");
    // }

    return maze;
}

