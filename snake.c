#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

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
void check_live(struct s_apple *, struct s_snake *);
void move_snake(struct s_snake *);
struct s_snake draw_snake();
struct s_apple draw_apple();





int main() {
    char c = ' ';
    int n = 0;
    int time = 500000;
    struct s_snake snake = draw_snake();
    struct s_apple apple = draw_apple();

    if (!isatty(STDIN_FILENO)) {
        if (freopen("/dev/tty", "r", stdin) == NULL) {
            printf("Cannot open the file");
        }
    }
    system("stty -icanon");

    while (c != 'q') {
        if (ioctl(0, FIONREAD, &n) == 0 && n > 0) {
            c = getchar();
            if (c == '=') time /= 4;
            if (c == '-') time *= 4;
            if (c == 'd') snake.direction = 1;
            if (c == 's') snake.direction = 2;
            if (c == 'a') snake.direction = 3;
            if (c == 'w') snake.direction = 4;
            if (c == 'q') break;
        }
        system("clear");
        move_snake(&snake);
        check_live(&apple, &snake);
        draw_garden(&apple, &snake);

        usleep(time);
    }


    return 0;
}

void check_live(struct s_apple * apple, struct s_snake * snake) {
    if (snake->x == apple->x && snake->y == apple->y) {
        *apple = draw_apple();
    }
    if (snake->x == WIDTH - 1 || snake->x == 0 || snake->y == HEIGHT - 1 || snake->y == 0) {
        *snake = draw_snake();
    }
}

void move_snake(struct s_snake * snake) {
    switch (snake->direction) {
        case 1:
            snake->x++;
            break;
        case 2:
            snake->y++;
            break;
        case 3:
            snake->x--;
            break;
        case 4:
            snake->y--;
            break;
        default:
            break;
    }
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
    //printf("snake-x = %d snake-y %d\n", snake->x, snake->y);
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
