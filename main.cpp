#include <stdio.h>
#include <graphics.h> //easyx图形库的头文件
#include <time.h>
#include "tools.h"
#include <math.h>

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib");	//音乐

//准备工作
//使用easyx图形库
//准备素材`


//开发日志
//1构建初始界面
//2构建方块数组
//3绘制初始方块数组
//4方块的移动



#define WIN_WIDHT			485
#define WIN_HEIGHT			717
#define ROWS				8
#define COLS				8
#define BLOCK_TYPE_COUNT	7	//方块种类
#define KIND_MOD			4	//使用方块种类	

IMAGE imgBg;	//表示背景图片
IMAGE imgBlocks[BLOCK_TYPE_COUNT];

//定义一个结构体类型，表示方块
struct block {
	int type;		//方块类型， 0：表示空白
	int x, y;		//位置
	int row, col;	//行，列
	int match;		//匹配次数
	int tmd;		//透明度：0-255，
};

struct block map[ROWS + 2][COLS + 2];

const int off_x = 17;		//最初位置
const int off_y = 200;		//274
const int block_size = 52;	//图片尺寸

int click = 0;	//计数器，单击两次交换
int posX1, posY1;	//两次单击位置
int posX2, posY2;

bool isMoving;	//是否正在移动
bool isSwap;	//当单击两个相邻的方块后，设置为true

int score;

void init() {
	//创建游戏窗口
	initgraph(WIN_WIDHT, WIN_HEIGHT);
	//initgraph(WIN_WIDHT, WIN_HEIGHT, 1);	

	loadimage(&imgBg, "res/bg2.png");
	char name[64];
	for (int i = 0; i < BLOCK_TYPE_COUNT; i++) {
		//res/1.png ...
		sprintf_s(name, sizeof(name), "res/%d.png", i + 1	);
		//		,block_size, block_size, true);		//缩放
		loadimage(&imgBlocks[i], name);
	}

	//配置随机数的随机种子
	srand(time(NULL));

	//初始化方块数组
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

	setFont("宋体", 20, 40);

	//播放背景音乐
	//mciSendString("play res/bg.mp3 repeat", 0, 0, 0);
	
	mciSendString("open res/bg.mp3 alias bgm", 0, 0, 0);
	mciSendString("play bgm res repat", 0, 0, 0);
	mciSendString("setaudio bgm volume to 80", 0, 0, 0); //设置声音
	mciSendString("play res/start.mp3", 0, 0, 0);
}

void updateWindow() {
	BeginBatchDraw(); //开始双缓冲
	putimage(0, 0, &imgBg);

	//绘制小方块
	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS; j++) {
			if (map[i][j].type != 0) {
				IMAGE* img = &imgBlocks[map[i][j].type - 1];
				//putimagePNG(map[i][j].x, map[i][j].y, img);	//处理图片
				putimageTMD(map[i][j].x, map[i][j].y, img, map[i][j].tmd);
			}
		}
	}

	char scoreStr[16];
	sprintf_s(scoreStr, sizeof(scoreStr), "%d", score);
	int x = 394 + (75 - strlen(scoreStr) * 20) / 2;//居中处理
	outtextxy(x, 64, scoreStr);	//打印分数

	EndBatchDraw(); //结束缓冲
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
	ExMessage msg;	//最新版本easyx图形库
	if (peekmessage(&msg) && msg.message == WM_LBUTTONDOWN) {
		//map[i][j].x = off_x + (j - 1) * (block_size + 5);
		//map[i][j].y = off_y + (i - 1) * (block_size + 5);
		//判断边界

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
				exchange(posY1, posX1, posY2, posX2);	//如果相邻，交换
				click = 0;

				isSwap = true;
				//播放音效
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
			for (int k = 0; k < 4; k++) {//加速
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
	//两个块相邻，发生移动 且 移动结束后
	if (isSwap && !isMoving) {
		//如果没有匹配到三个或三个以上的方块，就要还原
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
			if (map[i][j].match && map[i][j].tmd > 10) {//逐渐消失
				if (map[i][j].tmd == 255) {
					flag = true;
				}
				map[i][j].tmd -= 10;
				isMoving = true;		//先消除后下坠
			}
		}
	}
	if (flag) {
		PlaySound("res/clear.wav", 0, SND_FILENAME | SND_ASYNC);
	}
}

void updateGame() {
	if (isMoving)	return;
	//下坠
	for (int i = ROWS; i >= 0; i--) {
		for (int j = 1; j <= COLS; j++) {
			if (map[i][j].match) {//找到它上面第一个没有被消除的方块
				for (int k = i - 1; k >= 1; k--) {
					if (map[k][j].match == 0) {
						exchange(k, j, i, j);
						break;
					}
						
				}
			}
		}
	}

	//生成新方块，进行降落处理
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
		score += n;		//分数统计
	}
}


int main(void) {
	init();	//初始化

	updateWindow(); //更新窗口

	while (1) {
		userClick();	//处理用户的点击操作
		check();		//匹配次数检查
		move();			//移动
		xiaochu();		//消除
		huanYuan();		//还原
		updateWindow();	//游戏窗口的渲染，绘制

		updateGame(); //下坠 并 填充新方块

		if (isMoving)	Sleep(10);	//帧等待,判断移动优化
	}

	system("pause");	//暂停
	return 0;
}