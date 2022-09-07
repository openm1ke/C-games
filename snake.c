#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
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
#define SCORES "Scores"
#define LIVES "Lives"
#define GAMEOVER "Game over!"
#define MIDSCREENX (WIDTH / 2) - strlen(GAMEOVER) / 2
#define MIDSCREENY HEIGHT / 2

struct s_apple {
    int x;
    int y;
};

struct s_snake {
    int x;
    int y;
    int direction;
    struct s_snake * next;
};

void draw_garden(struct s_apple *, struct s_snake *, int *, int *);
struct s_snake * init_snake(struct s_snake *);
void move_snake(struct s_snake *);
//void draw_snake(struct s_snake *);
struct s_apple draw_apple();





int main() {
    char c = ' ';
    int n = 0;
    int time = 1000000;
    int scores = 0;
    int lives = 3;

    struct s_snake snake[4];
    struct s_snake * snake_head = init_snake(&snake[0]);
    /*
    printf("address %p - x = %d y = %d direction %d - next = %p\n", &snake[0], snake[0].x, snake[0].y, snake[0].direction, snake[0].next);
    printf("address %p - x = %d y = %d direction %d - next = %p\n", &snake[1], snake[1].x, snake[1].y, snake[1].direction, snake[1].next);
    printf("address %p - x = %d y = %d direction %d - next = %p\n", &snake[2], snake[2].x, snake[2].y, snake[2].direction, snake[2].next);
    printf("address %p - x = %d y = %d direction %d - next = %p\n", &snake[3], snake[3].x, snake[3].y, snake[3].direction, snake[3].next);
    exit(0);*/
    //draw_snake(snake_head);

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
            if (c == '=') time /= 2;
            if (c == '-') time *= 2;
            if (c == 'd') snake[0].direction = 1;
            if (c == 's') snake[0].direction = 2;
            if (c == 'a') snake[0].direction = 3;
            if (c == 'w') snake[0].direction = 4;
            if (c == 'q') break;
        }
        system("clear");
        draw_garden(&apple, snake_head, &scores, &lives);
        move_snake(snake_head);
        if (lives <= 0) break;
        usleep(time);
    }


    return 0;
}

void move_snake(struct s_snake * snake) {

    struct s_snake * temp = snake;
    while (temp != NULL) {
        switch (temp->direction) {
            case 1:
                temp->x++;
                break;
            case 2:
                temp->y++;
                break;
            case 3:
                temp->x--;
                break;
            case 4:
                temp->y--;
                break;
            default:
                break;
        }
        temp = temp->next;
    }

    temp = snake;
    struct s_snake * head = temp;
    temp = temp->next;
    while (temp != NULL) {
        struct s_snake * curr = temp;
        if(curr->direction != head->direction) {
            curr->direction = head->direction;
            head = temp->next;
        }

        temp = temp->next;
    }

}



struct s_snake * init_snake(struct s_snake * snake) {
    int d = 1;
    int x = WIDTH / 4;
    int y = HEIGHT - HEIGHT / 4;
    snake[0].x = x;
    snake[0].y = y;
    snake[0].direction = d;
    snake[0].next = &snake[1];

    snake[1].x = x-1;
    snake[1].y = y;
    snake[1].direction = d;
    snake[1].next = &snake[2];

    snake[2].x = x-2;
    snake[2].y = y;
    snake[2].direction = d;
    snake[2].next = &snake[3];

    snake[3].x = x-3;
    snake[3].y = y;
    snake[3].direction = d;
    snake[3].next = NULL;

    return snake;
}
/*
void draw_snake(struct s_snake * snake) {

}*/

struct s_apple draw_apple() {
    struct s_apple apple;
    srand(time(0));
    apple.x = (rand() % (WIDTH - 2)) + 1;
    apple.y = (rand() % (HEIGHT - 2)) + 1;
    return apple;
}

void draw_garden(struct s_apple * apple, struct s_snake * snake, int * scores, int * lives) {
    //printf("apple-x = %d apple-y %d\n", apple->x, apple->y);
    //printf("snake-x = %d snake-y %d\n", snake->x, snake->y);

    printf("address %p - x = %d y = %d direction %d - next = %p\n", &snake[0], snake[0].x, snake[0].y, snake[0].direction, snake[0].next);
    printf("address %p - x = %d y = %d direction %d - next = %p\n", &snake[1], snake[1].x, snake[1].y, snake[1].direction, snake[1].next);
    printf("address %p - x = %d y = %d direction %d - next = %p\n", &snake[2], snake[2].x, snake[2].y, snake[2].direction, snake[2].next);
    printf("address %p - x = %d y = %d direction %d - next = %p\n", &snake[3], snake[3].x, snake[3].y, snake[3].direction, snake[3].next);
    /*exit(0);*/
    struct s_snake * snk_temp;

    if (snake->x == apple->x && snake->y == apple->y) {
        *apple = draw_apple();
        *scores += 10;
    }
    if (snake->x == WIDTH - 1 || snake->x == 0 || snake->y == HEIGHT - 1 || snake->y == 0) {
        //*snake = draw_snake(snake);
        *lives -= 1;
    }
    printf("%s: %d\t%s: %d\n", SCORES, *scores, LIVES, *lives);

    for (int line = 0; line < HEIGHT; line++) {
        for (int column = 0; column < WIDTH; column++) {
            snk_temp = snake;
            while(snk_temp != NULL) {
                if(line == snk_temp->y && column == snk_temp->x) {
                    putchar(SNAKE);
                    break;
                }
                snk_temp = snk_temp->next;
            }
            if(snk_temp) {
                continue;
            }

            if (line == apple->y && column == apple->x) {
                putchar(APPLE);
            } else if (line == 0 || line == HEIGHT - 1) {
                putchar(WALL1);
            } else if (column == 0 || column == WIDTH - 1) {
                putchar(WALL2);
            } else if (line == MIDSCREENY && column == MIDSCREENX && *lives <= 0) {
                printf(GAMEOVER);
                column += strlen(GAMEOVER) - 1;
            } else {
                putchar(SPACE);
            }
        }
        putchar(NEWLINE);
    }
}
