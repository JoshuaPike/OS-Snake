#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "api.h"

// SNAKE RULES
// 1. If the snake passes the edge of the screen, the game ends.
// 2. If the snake's head collides with itself, the game ends.
// 3. If the snake's head collides with food, it grows longer.
// 4. Food is generated at a random location.

volatile int current_tick = 42;
int last_tick = 42;
volatile uint32_t kbd_input = 0x0;

int main() {
    setGraphicsMode();
    // Set color to sprite palette
    setColor(0, 0, 0xFF77FFC8); //snake color
    setColor(1, 0, 0xFF77005E); //food color

    int speed = 4; // how much each node moves per tick
    int snake_size = 16;
    int current_snake_node = 1;
    int x = 0;
    int y = 0;
    int center_x = x + 8;
    int center_y = y + 8;
    int all_x_nodes[128];
    int all_y_nodes[128];
    int full_length = 5;
    int current_length = 0;
    int alive = 1;
    uint32_t new_direction = 0;
    uint32_t last_direction = 0;
    int food_x = 200;
    int food_y = 200;
    int win = 0;

    while (alive == 1) {
        // subtract from food_x, food_y to account for offset of left corner
        // from the center
        current_tick = getTicks();
        setSmallSpriteControl(0, calcSmallSpriteControl(food_x-6,food_y-6,12,12,1));
        if(current_tick != last_tick & current_tick % speed == 0){
            // GET NEW DIRECTION
            kbd_input = getStatus();
            if (kbd_input == 0x0){ // no input
                new_direction = last_direction;
            }
            else {
                new_direction = kbd_input;
            }

            // UPDATE CURRENT COORDINATES
            if(new_direction & 0x1){
                x -= snake_size;
            }
            else if(new_direction & 0x2){
                y -= snake_size;
            }
            else if(new_direction & 0x4){
                y += snake_size;
            }
            else if(new_direction & 0x8){
                x += snake_size;
            }
            if((x > 511 | x < 0) + (y > 287 | y < 0)) {
                alive = 0; // game over; boundary passed
                break;
            }

            // CHECK SELF COLLISION
            all_x_nodes[current_snake_node - 1] = x; // update x and y values
            all_y_nodes[current_snake_node - 1] = y;

            for(int i = 1; i < current_length + 1; i++){
                if(i != current_snake_node){
                    if(all_x_nodes[i - 1] == x & all_y_nodes[i - 1] == y){
                        alive = 0;
                        break;
                    }
                }
            }

            // CHECK EAT FOOD
            center_x = x + 8; // center of current node
            center_y = y + 8;
            if(((food_x - center_x)*(food_x - center_x) < 14*14)
                & ((food_y - center_y)*(food_y - center_y) < 14*14)){ // this isn't working ever...
                full_length += 4;
                food_x = genRandom(512);
                food_y = genRandom(288);
            }

            
            // KEEP MOVING
            if (getSmallSpriteControl(current_snake_node) == 0x0){ // case where we extend snake
                setSmallSpriteControl(current_snake_node, calcSmallSpriteControl(x,y,snake_size,snake_size,0));
                current_length++;
            }
            else{
                shiftSmallSpriteControl(current_snake_node, x, y);
            }
            current_snake_node++; // go to next node of snake
            if (current_snake_node == full_length){ // we are at tail
                current_snake_node = 1; // go back to head
            }

            // WIN CONDITION
            if(current_length >= 128){
                alive = 0;
                win = 1;
            }

            last_tick = current_tick;
            last_direction = new_direction;
        }
    }
    // DELETE SNAKE SPRITES
    for(int i = 0; i < 128; i++){
        setSmallSpriteControl(i, 0x0);
    }

    // ENDGAME (does not work)
    // setTextMode();
    // int duration = 10;
    // if(current_tick != last_tick & duration > 0){
    //     if(win == 1){
    //         printLine("Congratulations! You crushed this game.");
    //     }
    //     else{
    //         printLine("Better luck next time.");
    //         }
    //     duration--;
    // }
    return 0;
}