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

void draw_board(int static_array[][HEIGHT]);

int main() {

    int static_array[WIDTH][HEIGHT] = {0};
    static_array[WIDTH-2][HEIGHT-2] = 1;
    static_array[1][HEIGHT-2] = 2;
    draw_board(static_array);

    return 0;
}


void draw_board(int static_array[][HEIGHT]) {
    for (int line = 0; line < HEIGHT; line++) {
        for (int column = 0; column < WIDTH; column++) {
            if (line == HEIGHT - 1) {
                putchar(WALL1);
            } else if (column == 0 || column == WIDTH - 1) {
                putchar(WALL2);
            } else if (line > 0 && line < HEIGHT - 1 && column > 0 && column < WIDTH - 1 && static_array[column][line] != 0) {
                printf("%d", static_array[column][line]);
            } else {
                putchar(SPACE);
            }
        }
        putchar(NEWLINE);
    }
}