#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "raylib.h"
#include "raymath.h"
#include "helpers.h"

typedef enum {
	RIGHT,
	LEFT,
	UP,
	DOWN
} Direction;


typedef enum {
	GAME,
	DEATH,
	MENU
} GamePhase;

typedef enum {
	MENU_BUTTON_START,
	MENU_BUTTON_SETTINGS,
	MENU_BUTTON_EXIT,
} MenuButton;

typedef enum {
	MENU_STATE_MAIN,
	MENU_STATE_SETTINGS
} MenuState;

Vector2 pos;
Vector2 vel;
Color BG_COLOR;
float speed, cell_size, x_offset, y_offset, padding;
Array snake;
Direction prev_dir;
Direction dir;
Queue move_queue;
GamePhase game_state = MENU;
float game_time;
int snake_size;
int score;


int menu_size = 3;
int menu_pos = 0;
float default_speed = 0.2;
int max_score = 0;
int apple_pos[2];
int move[] = {0, -1};
int width = 1280;
int height = 720;
int grid_size = 20;
MenuState menu_state = MENU_STATE_MAIN;

void set_score_text(char buffer[], char score_text[], int score){
    sprintf(buffer, "%s: %d", score_text, score);
}

bool SnakeIntersect(Array snake, int i, int j, int k){
	bool is_snake = false;
	for(int k = 0; k < snake_size; k++){
		is_snake = is_snake | (i == snake.array[k*2] & j == snake.array[k*2+1]);
	}
	return is_snake;
}

void GenerateApplePosition(){
	int i, j;
	do{
		j = rand()%grid_size;
		i = rand()%grid_size;
	}while(SnakeIntersect(snake, i, j, 0));
	apple_pos[0] = i;
	apple_pos[1] = j;
}

void ResetGameState(){
	snake_size = 5;
	initArray(&snake, 1);
	for(int i = 0; i < snake_size; i++){
		insertArray(&snake, grid_size/2);
		insertArray(&snake, grid_size/2+i);
	}
	score = 0;
	dir = UP;
	prev_dir = UP;
	speed = default_speed;
	game_time = 0;
	GenerateApplePosition();
	queue_init(&move_queue);
	queue_push(&move_queue, dir);
}


void DrawGameState(){
	float x, y;
	Color cell = {
		.r = 0xDF,
		.g = 0xDF,
		.b = 0xDF,
		.a = 0xFF
	};
	Color head = GetColor(0x2D4739FF);
	Color player = GetColor(0x09814aFF);
	Color apple = RED;
	Color current;
	for(int i = 0; i < grid_size; i++)
		for(int j = 0; j < grid_size; j++){
			x = i * cell_size + x_offset;
			y = j * cell_size + y_offset;
			bool is_snake = SnakeIntersect(snake, i, j, 0);
			if(i == snake.array[0] & j == snake.array[1]) {
				current = head;
			} else if (is_snake) {
				current = player;
			} else if (i == apple_pos[0] & j == apple_pos[1]){
				current = apple;
			} else {
				current = cell;
			}
			DrawRectangle(x + padding, y + padding, cell_size - padding, cell_size - padding, current);
		};

}

void DeathFrame(){ 
	ClearBackground(BG_COLOR);
	DrawGameState();
	DrawRectangle(0, 0, width, height, (Color){0, 0, 0, 0xDD});
	int font_size = 30;
	int text_width;
	int x_text_offset;
	int y_text_offset;
	char message[] = "You are dead! Press ENTER to continue.";
	text_width = MeasureText(message, font_size);
	x_text_offset = (width-text_width)/2;
	y_text_offset = (height-font_size)/2;
	DrawText(message, x_text_offset, y_text_offset, font_size, WHITE);

	char score_text[strlen("score: ") + 4];
	set_score_text(score_text, "score", score);
	text_width = MeasureText(score_text, font_size);
	x_text_offset = (width-text_width)/2;
	y_text_offset = font_size+font_size/2 + (height-font_size)/2;
	DrawText(score_text, x_text_offset, y_text_offset, font_size, WHITE);	

	char max_score_text[strlen("max_score: ") + 4];
	set_score_text(max_score_text, "max_score", max_score);
	text_width = MeasureText(max_score_text, font_size);
	x_text_offset = (width-text_width)/2;
	y_text_offset = 2*font_size+font_size/2 + (height-font_size)/2;
	DrawText(max_score_text, x_text_offset, y_text_offset, font_size, WHITE);	
}

void HandleMovements(){
	Direction new_dir;
	if(move_queue.used == 0){
		new_dir = prev_dir;
	} else {
		new_dir = queue_pop(&move_queue);
	}
	switch (new_dir){
		case UP:
			if(prev_dir == DOWN) return;
			move[0] = 0;
			move[1] = -1;
			break;
		case DOWN:
			if(prev_dir == UP) return;
			move[0] = 0;
			move[1] = 1;
			break;
		case LEFT:
			if(prev_dir == RIGHT) return;
			move[0] = -1;
			move[1] = 0;
			break;
		case RIGHT:
			if(prev_dir == LEFT) return;
			move[0] = 1;
			move[1] = 0;
			break;	
	}
	dir = new_dir;

}

void print_queue(Queue q){
	printf("%d", (int)q.used);
	for(int i = q.start; i<q.end; i++){
		printf("%d\n", (int)q.array[i%q.length]);
	}
}

void NewGameFrame(){ 
	ClearBackground(BG_COLOR);
	DrawGameState();
	char score_text[strlen("score: ") + 4];
	set_score_text(score_text, "score", score);
	char max_score_text[strlen("max_score: ") + 4];
	set_score_text(max_score_text, "max_score", max_score);
	DrawText(score_text, padding*5, padding*5, 32, WHITE);	
	DrawText(max_score_text, padding*5, 32+padding*5, 32, WHITE);	
	if (game_time > speed) {
		HandleMovements();
		int new_pos[] = {
			(snake.array[0] + move[0] + grid_size)%grid_size,
			(snake.array[1] + move[1] + grid_size)%grid_size,
		};
		if (SnakeIntersect(snake, new_pos[0], new_pos[1], 0)){
			game_state = DEATH;
			game_time = 0;
			return;
		};
		bool hit = new_pos[0] == apple_pos[0] & new_pos[1] == apple_pos[1];
		if(hit){
			score++;
			max_score = score > max_score ? score : max_score;
			snake_size++;
			speed = 0.2 - 0.03*log2(1+score);
			insertArray(&snake, 1);
			insertArray(&snake, 1);
			GenerateApplePosition();
		}
		for(int i = snake_size-1; i > 0; i--){
			snake.array[i*2+0] = snake.array[(i-1)*2+0];
			snake.array[i*2+1] = snake.array[(i-1)*2+1];
		}		
		game_time -= speed;
		snake.array[0] = new_pos[0];
		snake.array[1] = new_pos[1];
		prev_dir = dir;
	}
	
}

void GetMenuText(char menu_text[], int pos){
	MenuButton button = (MenuButton)pos;
	switch(pos){
		case MENU_BUTTON_START:
			sprintf(menu_text, "%s", "Start Game");
			break;
		case MENU_BUTTON_SETTINGS:
			sprintf(menu_text, "%s", "Settings");
			break;
		case MENU_BUTTON_EXIT:
			sprintf(menu_text, "%s", "Exit");
			break;
	}
}

void MenuFrame(){
	char menu_text[100];
	int font_size = 30;
	int text_width;
	int x_text_offset;
	int y_text_offset;
	Color col;
	for(int i = 0; i < menu_size; i++){
		if(i == menu_pos){
			col = YELLOW;	
		} else {
			col = WHITE;
		}
		GetMenuText(menu_text, i);
		text_width = MeasureText(menu_text, font_size);
		x_text_offset = (width-text_width)/2;
		y_text_offset = font_size*i + (height-menu_size*font_size)/2;
		DrawText(menu_text, x_text_offset, y_text_offset, font_size, col);
	}

}

void HandleKeys(){
	switch (game_state){
	case DEATH:
		if (IsKeyPressed(KEY_ENTER)){
			ResetGameState();
			game_state = GAME;
		}
		break;
	case GAME:
		if (IsKeyPressed(KEY_RIGHT)) queue_push(&move_queue, RIGHT);
		if (IsKeyPressed(KEY_LEFT)) queue_push(&move_queue, LEFT);
		if (IsKeyPressed(KEY_UP)) queue_push(&move_queue, UP); 
		if (IsKeyPressed(KEY_DOWN)) queue_push(&move_queue, DOWN); 
		break;
	case MENU:
		if (IsKeyPressed(KEY_UP)) menu_pos = (menu_pos - 1 + menu_size)%menu_size; 
		if (IsKeyPressed(KEY_DOWN)) menu_pos = (menu_pos + 1)%menu_size; 
		if (IsKeyPressed(KEY_ENTER)){
			switch( (MenuButton)menu_pos ){
				case MENU_BUTTON_START:
					game_state = GAME;
					ResetGameState();
					return;
				case MENU_BUTTON_EXIT:
					exit(0);

			}
		}
	}
}


int main(){
    InitWindow(width, height, "Snake!");
	SetTargetFPS(60);
	BG_COLOR = GetColor(0x181818FF);
	srand(time(NULL));

	ResetGameState();	
	while(!WindowShouldClose()){
		game_time += GetFrameTime();
        BeginDrawing();
		width = GetScreenWidth();
		height = GetScreenHeight();
		cell_size = (float)(width < height ? width : height) / grid_size;
		padding = 0.05 * cell_size;
		x_offset = (width < height ? 0 : ((float)width - height)/2);
		y_offset = (height < width ? 0 : ((float)height - width)/2);
		HandleKeys();
		switch(game_state){
			case DEATH:
				DeathFrame();
				break;
			case GAME:
				NewGameFrame();
				break;
			case MENU:
				MenuFrame();
				break;
		}
        EndDrawing();
    }
    return 1;
}