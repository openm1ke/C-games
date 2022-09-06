#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 80
#define HEIGHT 25
#define SNAKE '@'
#define WALL1 '-'
#define WALL2 '|'
#define SPACE ' '
#define APPLE 'o'
#define NEWLINE '\n'

struct s_apple {
    int x;
    int y;
};

struct s_snake {
    int x;
    int y;
    int direction;
};

void draw_garden(struct s_apple *, struct s_snake *);
struct s_snake draw_snake();
struct s_apple draw_apple();





int main() {

    struct s_snake snake = draw_snake();
    struct s_apple apple = draw_apple();
    draw_garden(&apple, &snake);

    return 0;
}

struct s_snake draw_snake() {
    struct s_snake snake;
    snake.x = WIDTH / 4;
    snake.y = HEIGHT - HEIGHT / 4;
    snake.direction = 1;
    return snake;
}

struct s_apple draw_apple() {
    struct s_apple apple;
    srand(time(0));
    apple.x = (rand() % (WIDTH - 2)) + 1;
    apple.y = (rand() % (HEIGHT - 2)) + 1;
    return apple;
}

void draw_garden(struct s_apple * apple, struct s_snake * snake) {
    //printf("apple-x = %d apple-y %d\n", apple->x, apple->y);
    printf("snake-x = %d snake-y %d\n", snake->x, snake->y);
    for (int line = 0; line < HEIGHT; line++) {
        for (int column = 0; column < WIDTH; column++) {
            if (line == apple->y && column == apple->x) {
                putchar(APPLE);
            } else if (line == snake->y && column == snake->x) {
                putchar(SNAKE);
            } else if (line == 0 || line == HEIGHT - 1) {
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
