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

struct s_snake * draw_garden(struct s_apple *, struct s_snake *, int *, int *);
struct s_snake * init_snake();
void move_snake(struct s_snake *);
struct s_snake * get_snake_head(struct s_snake *);
struct s_apple draw_apple();
struct s_snake * grow_snake(struct s_snake *);

int main() {
    char c = ' ';
    int n = 0;
    int time = 1000000;
    int scores = 0;
    int lives = 3;

    struct s_snake * snake;
    snake = init_snake();
    struct s_snake * snake_head = get_snake_head(snake);
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
            if (c == 'd') snake_head->direction = 1;
            if (c == 's') snake_head->direction = 2;
            if (c == 'a') snake_head->direction = 3;
            if (c == 'w') snake_head->direction = 4;
            if (c == 'q') break;
        }
        system("clear");

        snake = draw_garden(&apple, snake, &scores, &lives);
        move_snake(snake);
        snake_head = get_snake_head(snake);

        if (lives <= 0) break;
        usleep(time);
    }
    free(snake);

    return 0;
}

struct s_snake * get_snake_head(struct s_snake * head) {
    struct s_snake * temp = head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    return temp;
}

void move_snake(struct s_snake * snake) {
    struct s_snake * snake_tail = snake;
    while (snake_tail != NULL) {
        switch (snake_tail->direction) {
            case 1:
                snake_tail->x++;
                break;
            case 2:
                snake_tail->y++;
                break;
            case 3:
                snake_tail->x--;
                break;
            case 4:
                snake_tail->y--;
                break;
            default:
                break;
        }
        if (snake_tail->next)
            snake_tail->direction = snake_tail->next->direction;
        snake_tail = snake_tail->next;
    }
}


struct s_snake * init_snake() {
    struct s_snake * snake;
    snake = malloc(4 * sizeof (struct s_snake));
    int d = 1;
    int x = WIDTH / 4;
    int y = HEIGHT - HEIGHT / 4;
    snake[0].x = x;
    snake[0].y = y;
    snake[0].direction = d;
    snake[0].next = &snake[1];

    snake[1].x = x+1;
    snake[1].y = y;
    snake[1].direction = d;
    snake[1].next = &snake[2];

    snake[2].x = x+2;
    snake[2].y = y;
    snake[2].direction = d;
    snake[2].next = &snake[3];

    snake[3].x = x+3;
    snake[3].y = y;
    snake[3].direction = d;
    snake[3].next = NULL;
    return snake;
}

struct s_apple draw_apple() {
    struct s_apple apple;
    srand(time(0));
    apple.x = (rand() % (WIDTH - 2)) + 1;
    apple.y = (rand() % (HEIGHT - 2)) + 1;
    return apple;
}

struct s_snake * grow_snake(struct s_snake * snake) {
    struct s_snake * new_chain;
    new_chain = malloc(sizeof (struct s_snake));
    new_chain->next = snake;
    new_chain->x = snake->x;
    new_chain->y = snake->y;
    new_chain->direction = snake->direction;
    switch (snake->direction) {
        case 1:
            new_chain->x--;
            break;
        case 2:
            new_chain->y--;
            break;
        case 3:
            new_chain->x++;
            break;
        case 4:
            new_chain->y++;
            break;
        default:
            break;
    }

    return new_chain;
}
struct s_snake * draw_garden(struct s_apple * apple, struct s_snake * snake, int * scores, int * lives) {
    struct s_snake * tmp = snake;
    struct s_snake * snake_head = get_snake_head(tmp);

    if (snake_head->x == apple->x && snake_head->y == apple->y) {
        *apple = draw_apple();
        *scores += 10;
        snake = grow_snake(tmp);
        snake_head = get_snake_head(snake);
        free(tmp);
    }

    if (snake_head->x == WIDTH - 1 || snake_head->x == 0
    || snake_head->y == HEIGHT - 1 || snake_head->y == 0) {
        free(snake);
        snake = init_snake();
        snake_head = get_snake_head(snake);
        *lives -= 1;
    }
    tmp = snake;
    while (tmp != NULL) {
        if (snake_head->x == tmp->x && snake_head->y == tmp->y && snake_head != tmp) {
            free(snake);
            snake = init_snake();
            snake_head = get_snake_head(snake);
            *lives -= 1;
            break;
        }
        tmp = tmp->next;
    }

    printf("%s: %d\t%s: %d\n", SCORES, *scores, LIVES, *lives);

    for (int line = 0; line < HEIGHT; line++) {
        for (int column = 0; column < WIDTH; column++) {
            tmp = snake;
            while (tmp != NULL) {
                if (line == tmp->y && column == tmp->x) {
                    putchar(SNAKE);
                    break;
                }
                tmp = tmp->next;
            }
            if (tmp) {
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
    return snake;
}
