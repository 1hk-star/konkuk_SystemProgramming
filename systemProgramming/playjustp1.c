#include <stdio.h>
#include <string.h>

#define EXT_KEY			0xffffffe0	//확장키 인식값 
#define KEY_LEFT		0x4b
#define KEY_RIGHT		0x4d
#define KEY_UP			0x48
#define KEY_DOWN		0x50
void printMap(int map[10][10]) {
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			switch (map[i][j]) {
			case 0:
				printf("□");
				break;
			case 1:
				printf("■");
				break;
			case 2:
				printf("P1");
				break;
			case 3:
				printf("D1");
				break;
			case 4:
				printf("P2");
				break;
			case 5:
				printf("D2");
				break;
			case 6:
				printf("★");
			default:
				break;
			}
		}
		printf("\n");
	}

}
int main() {
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
		if (_kbhit())
		{
			system("cls");
			keytemp = _getch();
			if (keytemp == EXT_KEY)
			{
				keytemp = _getche();
				switch (keytemp)
				{
				case KEY_UP:		//상
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
				case KEY_DOWN:		//하
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
				case KEY_LEFT:		//좌
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
				case KEY_RIGHT:		//우
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


	return 0;
}
