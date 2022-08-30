#include <stdio.h>
#include <stdlib.h>

#define WIDTH 80
#define HEIGHT 25
#define SYMBOL '*'
#define WALL1 '-'
#define WALL2 '|'
#define SPACE ' '
#define NEWLINE '\n'

void draw_garden();

struct s_snake {
    int x;
    int y;
    int speed;
};

int main() {
    draw_garden();

    return 0;
}

void draw_garden() {
    for (int line = 0; line < HEIGHT; line++) {
        for (int column = 0; column < WIDTH; column++) {
            if (line == 0 || line == HEIGHT - 1) {
                putchar(WALL1);
            } else if (column == 0 || column == WIDTH - 1) {
                putchar(WALL2);
            } else {
                putchar(SPACE);
            }
        }
        putchar(NEWLINE);
    }
}
