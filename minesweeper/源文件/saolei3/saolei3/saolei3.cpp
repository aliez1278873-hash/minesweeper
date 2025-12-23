#pragma comment(linker, "/subsystem:windows /entry:mainCRTStartup")
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include <easyx.h>
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")

#define ROW 10
#define COL 10
#define BombNum 10
#define SIZE 50

IMAGE imgs[12];
void LoadResource() {
	for (int i = 0; i < 12; i++) {
		char imgPath[50] = { 0 };
		sprintf_s(imgPath, "./images/%d.jpg", i);
		loadimage(&imgs[i], imgPath, SIZE, SIZE);
	}
}
void show(int map[][COL]);
void init(int map[][COL]);
void draw(int map[][COL]);
void mouseMessage(ExMessage* msg, int map[][COL]);
void blank(int map[][COL], int row, int col);
int judge(int map[][COL], int row, int col);

int main() {
	initgraph(SIZE*ROW,SIZE*COL); 
	mciSendString("open ./images/music.mp3", NULL, 0, NULL);
	mciSendString("play ./images/music.mp3", NULL, 0, NULL);
	int map[10][10] = { 0 };
	init(map);
	while (1) {
		ExMessage msg;
		while (peekmessage(&msg)) {
			switch (msg.message) {
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
				mouseMessage(&msg, map);
				int ret = judge(map, msg.y / SIZE, msg.x / SIZE);
				if (ret == -1) {
					draw(map);
					int select = MessageBox(GetHWnd(), "You lose!One more time?", "System prompt.", MB_OKCANCEL);
					if (select == IDOK) {
						mciSendString("close ./images/music.mp3", NULL, 0, NULL);  
						mciSendString("open ./images/music.mp3", NULL, 0, NULL);  
						mciSendString("play ./images/music.mp3", NULL, 0, NULL);   
						init(map);
					}
					else {
						closegraph();
						mciSendString("close ./images/music.mp3", NULL, 0, NULL);
						exit(0);
					}
				}
				else if (ret == 1){
					draw(map);
					int select = MessageBox(GetHWnd(), "You win!One more time?", "System prompt.", MB_OKCANCEL);
					if (select == IDOK) {
						mciSendString("close ./images/music.mp3", NULL, 0, NULL);  
						mciSendString("open ./images/music.mp3", NULL, 0, NULL);  
						mciSendString("play ./images/music.mp3", NULL, 0, NULL);  
						init(map);
					}
					else {
						closegraph();
						mciSendString("close ./images/music.mp3", NULL, 0, NULL);
						exit(0);
					}
				}
				break;
			}
		}

		draw(map);
	}
	show(map);
	getchar();
	return 0;
}

void show(int map[][COL]) {
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			printf("%2d ", map[i][j]);
		}
		printf("\n");
	}
}

void init(int map[][COL]) {
	LoadResource();
	srand((unsigned)time(NULL));
	memset(map, 0, sizeof(int) * ROW * COL);
	for (int i = 0; i < BombNum; ) {
		int r = rand() % ROW;
		int c = rand() % COL;
		if (map[r][c] == 0) {
			map[r][c] = -1;
			i++;
		}
	}
	for (int i = 0; i < ROW; i++) {
		for (int k = 0; k < COL; k++) {
			if (map[i][k] == -1) {
				for (int r = i - 1; r <= i + 1; r++) {
					for (int m = k - 1; m <= k + 1; m++) {
						if ((r >= 0 && r < ROW && m >= 0 && m < COL) && (map[r][m] != -1)) {
							map[r][m]++;
						}
					}
				}
			}
		}
	}
	for (int i = 0; i < ROW; i++) {
		for (int k = 0; k < COL; k++) {
			map[i][k] += 20;
		}
	}
}

void draw(int map[][COL]) {
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++) {
			if (map[i][j] >= 0 && map[i][j] <= 8) {
				putimage(j * SIZE, i * SIZE, &imgs[map[i][j]]);
			}
			else if (map[i][j] == -1) {
				putimage(j * SIZE, i * SIZE, &imgs[9]);
			}
			else if (map[i][j] >= 19 && map[i][j] <= 28) {
				putimage(j * SIZE, i * SIZE, &imgs[10]);
			}
			else if (map[i][j] >= 39) {
				putimage(j * SIZE, i * SIZE, &imgs[11]);
			}
		}
	}
}

void mouseMessage(ExMessage* msg, int map[][COL]) {
	int r = msg->y / SIZE;
	int c = msg->x / SIZE;
	if (msg->message == WM_LBUTTONDOWN) {
		if (map[r][c] >= 19 && map[r][c] <= 28) {
			map[r][c] -= 20;
			blank(map, r, c);
		}
	}
	if (msg->message == WM_RBUTTONDOWN) {
		if (map[r][c] >= 19 && map[r][c] <= 28) {
			map[r][c] += 20;
		}
		else if (map[r][c] >= 39) {
			map[r][c] -= 20;
		}
	}
}

void blank(int map[][COL], int row, int col) {
	if (map[row][col] == 0) {
		for (int r = row - 1; r <= row + 1; r++) {
			for (int c = col - 1; c <= col + 1; c++) {
				if ((r >= 0 && r < ROW && c >= 0 && c < COL) && map[r][c] >= 19 && map[r][c] <= 28) {
					map[r][c] -= 20;
					blank(map, r, c);
				}
			}
		}
	}
}

int judge(int map[][COL], int row, int col) {
	if (map[row][col] == -1) {
		return -1;
	}
	int count = 0;
	for (int i = 0; i < ROW; i++) {
		for (int k = 0; k < COL; k++) {
			if (map[i][k] >= 0 && map[i][k] <= 8) {
				count++;
			}
		}
	}
	if (ROW * COL - BombNum == count) {
		return 1;
	}
	return 0;
}