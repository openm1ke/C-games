#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#define WIDTH 20
#define HEIGHT 15
#define WALL1 '-'
#define WALL2 '|'
#define SPACE ' '
#define NEWLINE '\n'

struct brick {
    int x;
    int y;
    int v1;
    int v2;
    int v3;
    int direction;
};

struct s_game {
    int scores;
    int ** board;
    struct brick * block;
};

void draw_board(struct s_game * game);

int main() {

    struct s_game game;
    game.scores = 0;

    //int static_array[WIDTH][HEIGHT] = {0};
    //static_array[WIDTH-2][HEIGHT-2] = 1;
    //static_array[1][HEIGHT-2] = 2;

    int **pointer_array = malloc(HEIGHT * sizeof(int *));
    for (int i = 0; i < WIDTH; i++) {
        pointer_array[i] = malloc(WIDTH * sizeof(int));
    }

    //printf("%d", pointer_array[1][1]);
    pointer_array[1][HEIGHT-2] = 2;
    //exit(0);
    game.board = pointer_array;

    struct brick * block;
    block = malloc(sizeof(struct brick));
    block->x = 3;
    block->y = 3;
    block->direction = 1;
    block->v1 = 1;
    block->v2 = 2;
    block->v3 = 3;

    game.block = block;
    //printf("%d", game.block->direction);
    //exit(0);
    system("clear");
    draw_board(&game);
    sleep(1);
    system("clear");
    block->y = 4;
    draw_board(&game);
    sleep(1);


    /*
    for (int i = 0; i < WIDTH; i++) {
        free(pointer_array[i]);
    }
    free(pointer_array);
    */
    //game.board = NULL;
    return 0;
}


void draw_board(struct s_game * game) {
    //printf("DRAW\n %d", game->block->direction);
    //exit(0);
    int block1x, block1y, block2x, block2y, block3x, block3y;
    switch (game->block->direction) {
        case 0:
            block1x = game->block->x - 1;
            block2x = game->block->x;
            block3x = game->block->x + 1;
            block1y = game->block->y;
            block2y = game->block->y;
            block3y = game->block->y;
            break;
        case 1:
            block1y = game->block->y - 1;
            block2y = game->block->y;
            block3y = game->block->y + 1;
            block1x = game->block->x;
            block2x = game->block->x;
            block3x = game->block->x;
            break;
    }

    //printf("%d %d %d %d %d %d", block1x, block2x, block3x, block1y, block2y, block3y);
    //exit(0);

    for (int line = 0; line < HEIGHT; line++) {
        for (int column = 0; column < WIDTH; column++) {
            if (line == HEIGHT - 1) {
                putchar(WALL1);
            } else if (column == 0 || column == WIDTH - 1) {
                putchar(WALL2);
            } else if (line == block1y && column == block1x) {
                printf("%d", game->block->v1);
            } else if (line == block2y && column == block2x) {
                printf("%d", game->block->v2);
            } else if (line == block3y && column == block3x) {
                printf("%d", game->block->v3);
            } else if (line > 0 && line < HEIGHT - 1 && column > 0 && column < WIDTH - 1 && game->board[column][line] != 0) {
                printf("%d", game->board[column][line]);
            } else {
                putchar(SPACE);
            }
        }
        putchar(NEWLINE);
    }
}