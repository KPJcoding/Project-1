#include <stdio.h>
#include <systick.h>
#include <stdlib.h>
#include <time.h>

//Size of grid for the game
#define width 30
#define height 48

//Different game states
typedef enum
{
  MENU,
  PLAYING,
  GAME_OVER,
  WIN
} Game_State;

//Food system and its coordinates
typedef struct
{
  int x;
  int y;
} Food;

//Body of snake and its coordinates
typedef struct
{
    int x;
    int y;
} bodypos;

//Direction of head of snake
typedef enum
{
    UP,
    DOWN,
    LEFT,
    RIGHT
}Direction;

//Datatype of snake
typedef struct
{
    bodypos body[1500];//for now here i have kept it 100 but we will need to keep the number according the size of led right which is 128*160 pixels
    int length;
    Direction dir;
    bodypos prev_tail;
    Game_State current_state;
} Snake;


void set_snake(Snake *snake)
{
  snake->length=3;
  snake->body[0].x=5;
  snake->body[0].y=5;
  snake->body[1].x=5;
  snake->body[1].y=4;
  snake->body[2].x=5;
  snake->body[2].y=3;
  snake->dir=RIGHT;
  snake->current_state=PLAYING;
}

uint8_t debounced_input() {
	    if (GPIOA->IDR & GPIO_IDR_ID1)
	    {          // Up
	        for(volatile int t=0; t<5000; t++);
	        if (GPIOA->IDR & GPIO_IDR_ID1) return 1;
	    }
	    else if (GPIOA->IDR & GPIO_IDR_ID4)
	    {     // Down
	        for(volatile int t=0; t<5000; t++);
	        if (GPIOA->IDR & GPIO_IDR_ID4) return 2;
	    }
	    else if (GPIOB->IDR & GPIO_IDR_ID0)
	    {     // Left
	        for(volatile int t=0; t<5000; t++);
	        if (GPIOB->IDR & GPIO_IDR_ID0) return 3;
	    }
	    else if (GPIOC->IDR & GPIO_IDR_ID1)
	    {     // Right
	        for(volatile int t=0; t<5000; t++);
	        if (GPIOC->IDR & GPIO_IDR_ID1) return 4;
	    }
	    return 0; // nothing pressed
	}

void game_input(Snake *snake)
{
	uint8_t input;
	input=debounced_input();

	if(input==1) input='w';
	else if(input==2) input='s';
	else if(input==3) input='a';
	else if(input==4) input='d';

	if(input=='w'&&snake->dir!=DOWN)
	{
		snake->dir=UP;
	}
	else if(input=='s'&&snake->dir!=UP)
	{
		snake->dir=DOWN;
	}
	else if(input=='a'&&snake->dir!=RIGHT)
	{
		snake->dir=LEFT;
	}
	else if(input=='d'&&snake->dir!=LEFT)
	{
		snake->dir=RIGHT;
	}
}

void spi_send(uint32_t data)
{
	    while(!(SPI2->SR & SPI_SR_TXE));
	    *(volatile uint8_t*)&SPI2->DR = data;
	    while(SPI2->SR & SPI_SR_BSY);
}

void lcd_cmd(uint32_t c)
{
	    GPIOC->BSRR = (1<<(6+16)); // A0/DC low = command
	    GPIOB->BSRR = (1<<(12+16)); // CS low
	    spi_send(c);
	    GPIOB->BSRR = (1<<12); // CS high
}

void lcd_data(uint32_t d)
{
	    GPIOC->BSRR = (1<<6); // A0/DC high = data
	    GPIOB->BSRR = (1<<(12+16)); // CS low
	    spi_send(d);
	    GPIOB->BSRR = (1<<12); // CS high
}


void move_snake(Snake *snake)
{

  for(int i=snake->length-1;i>0;i--)
  {
    if(i==snake->length-1)
    {
      snake->prev_tail=snake->body[i];//We need prev pos of tail as we have to increase size after eating apple
    }
    snake->body[i]=snake->body[i-1];
  }
  if(snake->dir==UP)
  {
    snake->body[0].y--;
  }
  if(snake->dir==DOWN)
  {
    snake->body[0].y++;
  }if(snake->dir==LEFT)
  {
    snake->body[0].x--;
  }if(snake->dir==RIGHT)
  {
    snake->body[0].x++;
  }

}
void incr_snake(Snake* snake)
{
  snake->body[snake->length-1]=snake->prev_tail;
}

int is_on_snake(Snake *snake,int x,int y)
{
  for(int i=0;i<snake->length;i++)
  {
    if(snake->body[i].x==x&&snake->body[i].y==y)
    {
      return 1;
    }

  }
  return 0;
}


void spawn_food(Snake *snake,Food *food)
{
 do
 {
   food->x=rand()%width;
   food->y=rand()%height;
 }
 while(is_on_snake(snake,food->x,food->y));
}


void check_food(Snake *snake,Food *food)
{
   if(snake->body[0].x==food->x&&snake->body[0].y==food->y)
   {
    snake->length++;
    spawn_food(snake,food);
    incr_snake(snake);
   }
}


void check_collision(Snake *snake)
{
  if(snake->body[0].x<0||snake->body[0].x>=width||snake->body[0].y<0||snake->body[0].y>=height)
  {
    snake->current_state=GAME_OVER;
  }

 for(int i=1;i<snake->length;i++)
 {
  if(snake->body[0].x==snake->body[i].x&&
  snake->body[0].y==snake->body[i].y)
   {
    snake->current_state=GAME_OVER;
   }
 }

}


void game_tick(Snake *snake,Food *food)
{
  move_snake(snake);
  check_food(snake,food);
  check_collision(snake);
}


void draw_snake(Snake *snake)
{
	int x1=(snake->prev_tail.x) *4;
	int y1=(snake->prev_tail.y) *4;

	lcd_cmd(0x2A);
	lcd_data(0); lcd_data(x1);
	lcd_data(0); lcd_data(x1+4);

	lcd_cmd(0x2B);
	lcd_data(0); lcd_data(y1);
	lcd_data(0); lcd_data(y1+4);

	lcd_cmd(0x2C);
	lcd_data(0xDB);
	lcd_data(0x00);
    for(int i = 0; i < snake->length; i++)
    {
    	int x2=(snake->body[i].x) *4;
    	int y2=(snake->body[i].y) *4;
        lcd_cmd(0x2A);
        lcd_data(0); lcd_data(x2);
        lcd_data(0); lcd_data(x2+4);

        lcd_cmd(0x2B);
        lcd_data(0); lcd_data(y2);
        lcd_data(0); lcd_data(y2+4);

        lcd_cmd(0x2C);
        lcd_data(0x00);
        lcd_data(0x00);
    }

}

void draw_food(Food *food)
{
	int x3=(food->x)*4;
	int y3=(food->y)*4;
    lcd_cmd(0x2A);
    lcd_data(0); lcd_data(x3);
    lcd_data(0); lcd_data(x3+4);


    lcd_cmd(0x2B);
    lcd_data(0); lcd_data(y3);
    lcd_data(0); lcd_data(y3+4);


    lcd_cmd(0x2C);
    lcd_data(0xF8);
    lcd_data(0x00);
}



void game_render(Snake *snake,Food *food)
{
  draw_snake(snake);
  draw_food(food);
}

void check_win(Snake *snake)
{
  if(snake->length==width*height)
  {
    snake->current_state=WIN;
  }
}
