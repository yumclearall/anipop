#include <stdio.h>
#include <graphics.h> //easyxͼ�ο��ͷ�ļ�
#include <time.h>
#include "tools.h"
#include <math.h>

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib");	//����

//׼������
//ʹ��easyxͼ�ο�
//׼���ز�`


//������־
//1������ʼ����
//2������������
//3���Ƴ�ʼ��������
//4������ƶ�



#define WIN_WIDHT			485
#define WIN_HEIGHT			717
#define ROWS				8
#define COLS				8
#define BLOCK_TYPE_COUNT	7	//��������
#define KIND_MOD			4	//ʹ�÷�������	

IMAGE imgBg;	//��ʾ����ͼƬ
IMAGE imgBlocks[BLOCK_TYPE_COUNT];

//����һ���ṹ�����ͣ���ʾ����
struct block {
	int type;		//�������ͣ� 0����ʾ�հ�
	int x, y;		//λ��
	int row, col;	//�У���
	int match;		//ƥ�����
	int tmd;		//͸���ȣ�0-255��
};

struct block map[ROWS + 2][COLS + 2];

const int off_x = 17;		//���λ��
const int off_y = 200;		//274
const int block_size = 52;	//ͼƬ�ߴ�

int click = 0;	//���������������ν���
int posX1, posY1;	//���ε���λ��
int posX2, posY2;

bool isMoving;	//�Ƿ������ƶ�
bool isSwap;	//�������������ڵķ��������Ϊtrue

int score;

void init() {
	//������Ϸ����
	initgraph(WIN_WIDHT, WIN_HEIGHT);
	//initgraph(WIN_WIDHT, WIN_HEIGHT, 1);	

	loadimage(&imgBg, "res/bg2.png");
	char name[64];
	for (int i = 0; i < BLOCK_TYPE_COUNT; i++) {
		//res/1.png ...
		sprintf_s(name, sizeof(name), "res/%d.png", i + 1	);
		//		,block_size, block_size, true);		//����
		loadimage(&imgBlocks[i], name);
	}

	//������������������
	srand(time(NULL));

	//��ʼ����������
	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS; j++) {
			map[i][j].type = 1 + rand() % KIND_MOD;
			map[i][j].row = i;
			map[i][j].col = j;
			map[i][j].x = off_x + (j - 1) * (block_size + 5);
			map[i][j].y = off_y + (i - 1) * (block_size + 5);
			map[i][j].match = 0;
			map[i][j].tmd = 255;
		}
	}

	click = 0;
	isMoving = false;
	isSwap = false;
	score = 0;

	setFont("����", 20, 40);

	//���ű�������
	//mciSendString("play res/bg.mp3 repeat", 0, 0, 0);
	
	mciSendString("open res/bg.mp3 alias bgm", 0, 0, 0);
	mciSendString("play bgm res repat", 0, 0, 0);
	mciSendString("setaudio bgm volume to 80", 0, 0, 0); //��������
	mciSendString("play res/start.mp3", 0, 0, 0);
}

void updateWindow() {
	BeginBatchDraw(); //��ʼ˫����
	putimage(0, 0, &imgBg);

	//����С����
	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS; j++) {
			if (map[i][j].type != 0) {
				IMAGE* img = &imgBlocks[map[i][j].type - 1];
				//putimagePNG(map[i][j].x, map[i][j].y, img);	//����ͼƬ
				putimageTMD(map[i][j].x, map[i][j].y, img, map[i][j].tmd);
			}
		}
	}

	char scoreStr[16];
	sprintf_s(scoreStr, sizeof(scoreStr), "%d", score);
	int x = 394 + (75 - strlen(scoreStr) * 20) / 2;//���д���
	outtextxy(x, 64, scoreStr);	//��ӡ����

	EndBatchDraw(); //��������
}

void exchange(int row1, int col1, int row2, int col2) {
	struct block tmp = map[row1][col1];
	map[row1][col1] = map[row2][col2];
	map[row2][col2] = tmp;

	map[row1][col1].row = row1;
	map[row1][col1].col = col1;
	map[row2][col2].row = row2;
	map[row2][col2].col = col2;

}

void userClick(void) {
	ExMessage msg;	//���°汾easyxͼ�ο�
	if (peekmessage(&msg) && msg.message == WM_LBUTTONDOWN) {
		//map[i][j].x = off_x + (j - 1) * (block_size + 5);
		//map[i][j].y = off_y + (i - 1) * (block_size + 5);
		//�жϱ߽�

		if (msg.x < off_x || msg.y < off_y) return;

		int col = (msg.x - off_x) / (block_size + 5) + 1;
		int row = (msg.y - off_y) / (block_size + 5) + 1;

		if (col > COLS || row > ROWS) return;
		//printf("%d,%d\n", col, row);

		click++;
		if (click == 1) {
			posX1 = col;
			posY1 = row;
		}
		else if(click == 2){
			posX2 = col;
			posY2 = row;

			if (abs(posX1 - posX2) + abs(posY1 - posY2) == 1) {
				exchange(posY1, posX1, posY2, posX2);	//������ڣ�����
				click = 0;

				isSwap = true;
				//������Ч
				//mciSendString("res/pao.wav", 0, 0, 0);
				PlaySound("res/pao.wav", 0, SND_FILENAME | SND_ASYNC);

			}
			else {
				click = 1;
				posX1 = col;
				posY1 = row;
			}

		}

	}
}
void move() {
	isMoving = false;
	for (int i = ROWS; i > 0; i--) {
		for (int j = 1; j <= COLS; j++) {
			struct block* p = &map[i][j];
			int dx, dy;
			for (int k = 0; k < 4; k++) {//����
				p->x;
				p->y;
				int x = off_x + (p->col - 1) * (block_size + 5);
				int y = off_y + (p->row - 1) * (block_size + 5);

				dx = p->x - x;
				dy = p->y - y;

				if (dx) p->x -= dx / abs(dx);
				if (dy) p->y -= dy / abs(dy);
			}

			if (dx || dy)	isMoving = true;

		}
	}
}
void huanYuan() {
	//���������ڣ������ƶ� �� �ƶ�������
	if (isSwap && !isMoving) {
		//���û��ƥ�䵽�������������ϵķ��飬��Ҫ��ԭ
		int count = 0;
		for (int i = ROWS; i > 0; i--) {
			for (int j = 1; j <= COLS; j++) {
				count += map[i][j].match;
			}
		}

		if (count == 0) {
			exchange(posY1, posX1, posY2, posX2);
		}
		isSwap = false;
	}
}

void check() {
	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS; j++) {
			if (map[i][j].type == map[i + 1][j].type
				&& map[i][j].type == map[i - 1][j].type) {
				map[i][j].match++;
				map[i + 1][j].match++;
				map[i - 1][j].match++;
			}
			if (map[i][j].type == map[i][j + 1].type
				&& map[i][j].type == map[i][j - 1].type) {
				for (int k = -1; k < 2; k++)	map[i][j + k].match++;
			}
		}
	}
}
void xiaochu() {
	if (isMoving)	return;
	bool flag = false;
	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS; j++) {
			if (map[i][j].match && map[i][j].tmd > 10) {//����ʧ
				if (map[i][j].tmd == 255) {
					flag = true;
				}
				map[i][j].tmd -= 10;
				isMoving = true;		//����������׹
			}
		}
	}
	if (flag) {
		PlaySound("res/clear.wav", 0, SND_FILENAME | SND_ASYNC);
	}
}

void updateGame() {
	if (isMoving)	return;
	//��׹
	for (int i = ROWS; i >= 0; i--) {
		for (int j = 1; j <= COLS; j++) {
			if (map[i][j].match) {//�ҵ��������һ��û�б������ķ���
				for (int k = i - 1; k >= 1; k--) {
					if (map[k][j].match == 0) {
						exchange(k, j, i, j);
						break;
					}
						
				}
			}
		}
	}

	//�����·��飬���н��䴦��
	for (int i = ROWS; i >= 0; i--) {
		int n = 0;
		for (int j = 1; j <= COLS; j++) {
			if (map[i][j].match) {
				map[i][j].type = 1 + rand() % KIND_MOD;
				map[i][j].y = off_y - (n + 1) * (block_size + 5);
				n++;
				map[i][j].match = 0;
				map[i][j].tmd = 255;
			}
		}
		score += n;		//����ͳ��
	}
}


int main(void) {
	init();	//��ʼ��

	updateWindow(); //���´���

	while (1) {
		userClick();	//�����û��ĵ������
		check();		//ƥ��������
		move();			//�ƶ�
		xiaochu();		//����
		huanYuan();		//��ԭ
		updateWindow();	//��Ϸ���ڵ���Ⱦ������

		updateGame(); //��׹ �� ����·���

		if (isMoving)	Sleep(10);	//֡�ȴ�,�ж��ƶ��Ż�
	}

	system("pause");	//��ͣ
	return 0;
}