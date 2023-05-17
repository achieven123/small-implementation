#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include<time.h>

#pragma region Key Input

#define UP 72
#define DOWN 80
#define LEFT 75
#define RIGHT 77

#define W 119
#define S 115
#define A 97
#define D 100

#define ESC 27
#define ENTER 13

#pragma endregion

#pragma region Map Size

#define GAP_X 4
#define GAP_Y 2
#define MAX_WIDTH 100
#define MAX_HEIGHT 100

#pragma endregion

#pragma region Enum & Struct

enum {
	BLACK,
	D_BLUE,
	D_GREEN,
	D_SKYBLUE,
	D_RED,
	D_VIOLET,
	D_YELLOW,
	GRAY,
	D_GRAY,
	BLUE,
	GREEN,
	SKYBLUE,
	RED,
	VIOLET,
	YELLOW,
	WHITE
};

typedef int element;

typedef struct ListNode {
	element x;
	element y;
	ListNode* left;
	ListNode* right;
} ListNode;

#pragma endregion  

#pragma region Function Prototype

void set_color(int back_color, int font_color);
void gotoxy(int x, int y);
void set_block(int x, int y, int color);
void delete_block(int x, int y);

void init_game();
void draw_window();
void draw_map();
void draw_info();

int input_key();
void game_start();
void init_list();
void insert_first(int dir);
void insert_end(int dir);
void delete_end();

void create_fruit();

#pragma endregion 

int map[MAX_HEIGHT][MAX_WIDTH];
int snake[MAX_HEIGHT][MAX_WIDTH];
int fruit[MAX_HEIGHT][MAX_WIDTH];
int map_width = 25;
int map_height = 25;
int speed = 50;
int current_score;
int max_score;

ListNode* head;
ListNode* tail;

int main() {
	init_game();
	draw_window();
	draw_map();
	draw_info();
	gotoxy(0, 0);
	printf("안녕하세요");
	game_start();

	_getch();

	return 0;
}

void set_color(int back_color, int font_color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), back_color * 16 + font_color);
}

void gotoxy(int x, int y) {
	COORD pos = { x + GAP_X + 2, y + GAP_Y + 4 };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void set_block(int x, int y, int color) {
	set_color(color, WHITE);
	gotoxy(x * 2, +y);
	printf("  ");
	set_color(BLACK, WHITE);
}

void delete_block(int x, int y) {
	if (map[x][y] == 0) set_block(x, y, GREEN);
	else set_block(x, y, D_GREEN);
}

void init_game() {
	//커서 숨기기
	CONSOLE_CURSOR_INFO cursorInfo = { 0, };
	cursorInfo.bVisible = FALSE;
	cursorInfo.dwSize = 1;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);

	//윈도우 창 최대화 메시지
	system("cls");
	set_color(BLACK, WHITE);
	printf("Set the console window to full screen and press any key...");
	getch();
	system("cls");

	//맵 배열 생성
	for (int y = 0; y < MAX_HEIGHT; y++) {
		for (int x = 0; x < MAX_WIDTH; x++) {
			if ((x + y) % 2 != 0)  map[y][x] = 1;
		}
	}
}

void draw_window() {
	for (int x = -1, y = -4; x < map_width + 1; x++) {
		set_block(x, y, D_GRAY);
		set_block(x, y + 1, D_GRAY);
		set_block(x, y + 2, D_GRAY);
		set_block(x, y + 3, GRAY);
		set_block(x, y + map_height + 4, GRAY);
	}

	for (int x = -1, y = 0; y < map_height; y++) {
		set_block(x, y, GRAY);
		set_block(x + map_width + 1, y, GRAY);
	}
}

void draw_map() {
	for (int y = 0; y < map_height; y++) {
		for (int x = 0; x < map_width; x++) {
			delete_block(x, y);
		}
	}
}

void draw_info() {
	int x = 0;
	int y = -3;

	set_color(D_GRAY, WHITE);
	gotoxy(x, y); printf("score %d", current_score);
	gotoxy(x + map_width * 2 - 15, y); printf("20223070 김경훈");

	x = map_width * 2 + 6;
	y = map_height - 6;

	set_color(BLACK, WHITE);
	gotoxy(x, y); printf("High Socre : %d", max_score);
	gotoxy(x, y + 2); printf("Move Up    : W or ↑");
	gotoxy(x, y + 3); printf("Move Down  : S or ↓");
	gotoxy(x, y + 4); printf("Move Left  : A or ←");
	gotoxy(x, y + 5); printf("Move Right : D or →");
	gotoxy(x, y + 6); printf("Game Stop	 : ESC");
}

int input_key() {
	int input = _getch();

	if (input == 224) { input = _getch(); return input; }
	else if (input == W) return UP;
	else if (input == S) return DOWN;
	else if (input == A) return LEFT;
	else if (input == D) return RIGHT;
}

void game_start() {
	srand(time(NULL));

	int dir = RIGHT;
	int a = 0;
	int input;

	init_list();
	create_fruit();
	set_color(BLACK, GREEN);
	gotoxy(-2, map_height + 2);
	printf("시작하려면 아무 키나 누르십시오 . . .");
	_getch();
	set_color(BLACK, WHITE);
	gotoxy(-2, map_height + 2);
	for (int y = 0; y < 50; y++) printf(" ");

	while (true) {
		if (_kbhit()) {
			input = input_key();

			switch (input) {
			case UP:
			case DOWN:
			case LEFT:
			case RIGHT:
				if ((input != UP && dir == DOWN) ||
					(input != DOWN && dir == UP) ||
					(input != LEFT && dir == RIGHT) ||
					(input != RIGHT && dir == LEFT)) {
					dir = input;
				}
				break;

			case ESC:
				set_color(BLACK, RED);
				gotoxy(-2, map_height + 2);
				printf("일시정지 : 계속하려면 아무 키나 누르십시오 . . .");
				for (int y = 0; y < map_height; y++) {
					for (int x = 0; x < map_width; x++) {
						printf("%d", snake[y][x]);
					}
					printf("\n");
				}
				_getch();
				set_color(BLACK, WHITE);
				gotoxy(-2, map_height + 2);
				for (int y = 0; y < 50; y++) printf(" ");

				break;
			}
		}
		
		if (fruit[head->y][head->x] == 1) {
			fruit[head->y][head->x] = 0;
			insert_end(dir);
			create_fruit();

			current_score++;
			draw_info();
		}

		if (snake[head->y][head->x] == 1) {
			set_color(BLACK, RED);
			gotoxy(-2, map_height + 2);
			printf("Game Over");
			_getch();
		}
		if (head->x < 0 || head->x >= map_width || head->y < 0 || head->y >= map_height) {
			set_color(BLACK, RED);
			gotoxy(-2, map_height + 2);
			printf("Game Over");
			break;
		}

		insert_first(dir);
		delete_end();
		Sleep(speed);

	}
}

void init_list() {
	ListNode* new_node = (ListNode*)malloc(sizeof(ListNode));

	new_node->x = map_width / 2;
	new_node->y = map_height / 2;
	new_node->left = NULL;
	new_node->right = NULL;

	head = new_node;
	tail = new_node;

	set_block(head->x, head->y, BLUE);
}

void insert_first(int dir) {
	ListNode* new_node = (ListNode*)malloc(sizeof(ListNode));

	new_node->x = head->x;
	new_node->y = head->y;

	if (dir == UP) new_node->y--;
	if (dir == DOWN) new_node->y++;
	if (dir == LEFT) new_node->x--;
	if (dir == RIGHT) new_node->x++;

	new_node->left = NULL;
	new_node->right = head;
	head->left = new_node;
	head = new_node;

	set_block(head->x, head->y, BLUE);
	set_block(head->right->x, head->right->y, SKYBLUE);
	snake[head->right->y][head->right->x] = 1;
}

void insert_end(int dir) {
	ListNode* new_node = (ListNode*)malloc(sizeof(ListNode));

	new_node->x = tail->x;
	new_node->y = tail->y;

	if (dir == UP) new_node->y++;
	if (dir == DOWN) new_node->y--;
	if (dir == LEFT) new_node->x++;
	if (dir == RIGHT) new_node->x--;

	new_node->left = tail;
	new_node->right = NULL;
	tail->right = new_node;
	tail = new_node;

	set_block(tail->x, tail->y, SKYBLUE);
	snake[tail->y][tail->x] = 1;
}

void delete_end() {
	ListNode* removed;

	delete_block(tail->x, tail->y);

	removed = tail;
	removed->left->right = NULL;
	tail = removed->left;

	snake[removed->y][removed->x] = 0;

	free(removed);
}

void create_fruit() {
	int x = rand() % map_width;
	int y = rand() % map_height;

	fruit[y][x] = 1;
	set_block(x, y, RED);
}