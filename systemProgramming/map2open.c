nclude <stdio.h>

int main() {
	int map[10][10] = { 
	{5,0,0,0,0,0,0,0,0,3}, //0
	{1,1,1,0,0,0,0,1,1,1}, //1
	{0,0,0,0,0,0,0,0,0,0}, //2
	{0,1,1,1,1,1,1,1,1,0}, //3
	{0,0,0,0,1,0,0,0,0,0}, //4
	{0,0,1,0,0,1,0,1,0,0}, //5
	{1,0,0,0,1,0,0,0,0,1}, //6
	{0,0,0,0,0,1,0,0,0,0}, //7
	{0,1,0,0,0,0,0,0,1,0}, //8
	{2,1,0,0,0,0,0,0,1,4}, //9
	};
	//플레이어 시작 위치, 도착 위치는 2 3 4 5로 설정해놓음.

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
			default:
				break;
			}
		}
		printf("\n");
	}
}