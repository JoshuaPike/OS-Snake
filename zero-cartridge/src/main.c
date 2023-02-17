#include <stdint.h>
#include <string.h>
#include "video_api.h"

volatile int global = 42;
volatile uint32_t controller_status = 0;

volatile char *VIDEO_MEMORY = (volatile char *)(0x50000000 + 0xFE800);

volatile uint32_t *SPRITE_PALLETE = (volatile uint32_t *)(0x500FD000);
volatile uint32_t *LARGE_SPRITE_CONTROL = (volatile uint32_t *)(0x500FF114);
volatile uint32_t *VIDEO_MODE = (volatile uint32_t *)(0x500FF414);
volatile uint32_t *LARGE_SPRITE_DATA = (volatile uint32_t *)(0x500B4000);
volatile uint32_t *SMALL_SPRITE_CONTROL = (volatile uint32_t *)(0x500FF214);

volatile uint32_t *INTERRUPT_ENABLE = (volatile uint32_t *)(0x40000000);
volatile uint32_t *INTERRUPT_PENDING = (volatile uint32_t *)(0x40000004);

int playSnake();

struct Food{
    int x;
    int y;
};

struct Snake{
    int x[127];
    int y[127];
};

enum Direction{
    UP,
    DOWN,
    RIGHT,
    LEFT
};

void clearTextScreen(){
    for (int i = 0; i < 2240; i++){
        VIDEO_MEMORY[i] = ' ';
    }
}

void updateDirection(enum Direction *direction, struct Snake *snake){
    if(*direction == RIGHT){
        snake->x[0]++;
        return;
    } else if (*direction == LEFT){
        snake->x[0]--;
        return;
    } else if (*direction == UP){
        snake->y[0] -= 0x40;
        return;
    } else if (*direction == DOWN){
        snake->y[0] += 0x40;
        return;
    }
}

void follow(int length, struct Snake *snake){
    for (int i = length; i > 0; i--){
        snake->x[i] = snake->x[i-1];
        snake->y[i] = snake->y[i-1];
    }
}

void drawSnake(int length, struct Snake *snake){
    //count mod num
    follow(length, snake);
    for (int i = 0; i <= length; i++){
        VIDEO_MEMORY[snake->x[i] + snake->y[i]] = 'X';
    }
    VIDEO_MEMORY[snake->x[0] + snake->y[0]] = 'O';
}

void refreshSnake(int length, struct Snake *snake){
    for (int i = 0; i <= length; i++){
        VIDEO_MEMORY[snake->x[i] + snake->y[i]] = ' ';
    }
}

int foodCollision(struct Food *food, struct Snake *snake){
    return (food->x == snake->x[0]) && (food->y == snake->y[0]);
}

int selfCollision(int length, struct Snake *snake){
    for (int i = 1; i <= length; i++){
        if((snake->x[i] == snake->x[0]) && (snake->y[i] == snake->y[0])){
            return 1;
        }
    }
    return 0;
}

int boundCheck(struct Snake *snake){
    if (snake->y[0] < 0){
        return 1;
    }
    if (snake->y[0] > 2240){
        return 1;
    }
    if (snake->x[0] < 0){
        return 1;
    }
    if (snake->x[0] > 63){
        return 1;
    }
    return 0;
}

void displayScore(int score, int pos){
    VIDEO_MEMORY[pos] = score;
    int ctr = score;
    while(ctr) {
        VIDEO_MEMORY[pos] = (ctr%10) + '0';
        ctr /= 10;
        pos--;
    }
}

void displayCurrentScore(int score){
    int pos = 60 + 2112; // x + y
    displayScore(score, pos);
}

void endGame(int score){
    int pos = 28 + 0x480; //set it middle
    char gameOver[] = {'G','A','M','E',' ','O','V','E','R'};
    char scoreText[] = {'S','C','O','R','E',':',' '};
    clearTextScreen();

    // print game over
    for (int i = 0; i < 9; i++){
        VIDEO_MEMORY[pos + i] = gameOver[i];
    }
    // print score label
    pos += 64; // move a unit down
    for (int i = 0; i < 7; i++){
        VIDEO_MEMORY[pos + i] = scoreText[i];
    }
    pos += 8;  //print to right of score
    displayScore(score, pos);

    int last_global = 42;
    int delay = 0;

    while (delay < 60) {
        global = getTicks();
        if(global != last_global){
            delay++;
        }
        last_global = global;
    }
    clearTextScreen();
    playSnake();
}

void someWhatRandomFood(struct Food *food){
    int begin = getTicks();
    int x = 1; // start at 1 so food wont touch wall
    int y = 64; // start one unit down
    while(begin > 33) {
        begin /= 10;
    }
    for (int i = 0; i < begin; i++){
        y += 64;
    }

    begin = getTicks();
    while(begin > 64) {
        begin /= 10;
    }
    for (int i = 0; i < begin; i++){
        x++;
    }
    
    food->x = x;
    food->y = y;
}


int playSnake() {
    int last_global = 42;
    int delay = 0;

    struct Food food = {32,0x480};
    struct Snake snake;
    enum Direction direction = RIGHT;

    // initial snake position
    snake.x[0] = 1;
    snake.y[0] = 0x40;

    int length = 2;
    follow(length, &snake); //intialize at length

    while (1) {
        global = getTicks();
        controller_status = getStatus();
        VIDEO_MEMORY[food.x + food.y] = 'f';
        
        if(global != last_global){ 
            if(controller_status){ // Block actions until current action is completed.
                if(controller_status & 0x1){
                    // left
                    if (direction != RIGHT) direction = LEFT;
                }
                if(controller_status & 0x2){
                    // up
                    if (direction != DOWN) direction = UP;
                }
                if(controller_status & 0x4){
                    // down
                    if (direction != UP) direction = DOWN;
                }
                if(controller_status & 0x8){
                    // right
                    if (direction != LEFT) direction = RIGHT;
                }
            }
        
            if (delay > 2){
                if(foodCollision(&food, &snake)){
                    someWhatRandomFood(&food);
                    length++;
                }
                refreshSnake(length, &snake);
                updateDirection(&direction, &snake);
                if(selfCollision(length, &snake) || boundCheck(&snake)){
                    endGame(length - 1);
                    return 0;
                }
                drawSnake(length, &snake);
                displayCurrentScore(length-1);
            delay = 0;
            }
            last_global = global;
            delay++;
        }
    }
    return 0;
}

int main(){
    playSnake();
    return 0;
}
