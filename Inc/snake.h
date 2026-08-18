#ifndef SNAKE_H
#define SNAKE_H
#include <stdint.h>

#define width 30
#define height 50

typedef enum
{
	MENU, PLAYING, GAME_OVER, WIN
	} Game_State;

typedef struct
{
	  int x;
	  int y;
} Food;

typedef struct{
    int x;
    int y;
} bodypos;


typedef enum{
    UP,
    DOWN,
    LEFT,
    RIGHT
}Direction;

typedef struct{
    bodypos body[1500];
    int length;
    Direction dir;
    bodypos prev_tail;
    Game_State current_state;
} Snake;

void set_snake(Snake *snake);
void game_input(Snake *snake);
void move_snake(Snake *snake);
void incr_snake(Snake* snake);
void spawn_food(Snake *snake,Food *food);
void check_food(Snake *snake,Food *food);
void check_collision(Snake *snake);
void game_tick(Snake *snake,Food *food);
void draw_snake(Snake *snake);
void draw_food(Food *food);
void game_render(Snake *snake,Food *food);
void check_win(Snake *snake);
int is_on_snake(Snake *snake,int x,int y);
uint8_t debounced_input(void);
void spi_send(uint8_t data);
void lcd_cmd(uint8_t c);
void lcd_data(uint8_t d);
#endif


