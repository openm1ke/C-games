#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#define WIDTH 80
#define HEIGHT 25
#define SNAKE 'Q'
#define BODY 'o'
#define WALL1 '-'
#define WALL2 '|'
#define SPACE ' '
#define APPLE '*'
#define NEWLINE '\n'
#define SPEED "Speed"
#define SCORES "Scores"
#define LIVES "Lives"
#define GAMEOVER "Game over!"
#define SPEED_START 100000
#define SPEED_INC 5000
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
    struct s_snake *next;
};
struct s_game {
    int scores;
    int lives;
    int speed;
    struct s_apple *apple;
    struct s_snake *snake;
    struct s_snake *snake_head;
};

void play(struct s_game *);
void new_snake(struct s_game *);
void move_snake(struct s_snake *);
void init_game(struct s_game *);
struct s_apple *init_apple();
struct s_snake *init_snake();
struct s_snake *get_snake_head(struct s_snake *);
struct s_snake *add_snake(struct s_snake *, int);
void apple_destroy(struct s_apple *);
void snake_destroy(struct s_snake *);

int main() {
    char c = ' ';
    int n = 0;
    int d;
    struct s_game game;

    init_game(&game);

    if (!isatty(STDIN_FILENO)) {
        if (freopen("/dev/tty", "r", stdin) == NULL) {
            printf("Cannot open the file");
        }
    }
    system("stty -icanon");

    while (c != 'q') {
        if (ioctl(0, FIONREAD, &n) == 0 && n > 0) {
            c = getchar();
            d = game.snake_head->direction;
            if (c == '=' || c == '+') game.speed -= SPEED_INC;
            if (c == '-') game.speed += SPEED_INC;
            if (c == 'd' && d != 3) game.snake_head->direction = 1;
            if (c == 's' && d != 4) game.snake_head->direction = 2;
            if (c == 'a' && d != 1) game.snake_head->direction = 3;
            if (c == 'w' && d != 2) game.snake_head->direction = 4;
            if (c == 'q') break;
        }
        system("clear");

        play(&game);

        if (game.lives <= 0) break;
        usleep(game.speed);
    }
    apple_destroy(game.apple);
    snake_destroy(game.snake);
    return 0;
}

void init_game(struct s_game *game) {
    game->scores = 0;
    game->lives = 3;
    game->speed = SPEED_START;
    game->apple = init_apple();
    game->snake = init_snake();
    add_snake(add_snake(add_snake(game->snake, -1), -1), -1);
    game->snake_head = get_snake_head(game->snake);
}

void play(struct s_game *game) {
    struct s_snake *temp;
    move_snake(game->snake);

    if (game->snake_head->x == game->apple->x && game->snake_head->y == game->apple->y) {
        apple_destroy(game->apple);
        game->apple = init_apple();
        game->scores += 10;
        //  game->speed -= SPEED_INC;
        game->snake = add_snake(game->snake, 1);
    }

    if (game->snake_head->x == WIDTH - 1 || game->snake_head->x == 0 || game->snake_head->y == HEIGHT - 1 ||
        game->snake_head->y == 0) {
        game->lives -= 1;
        new_snake(game);
    }

    temp = game->snake;
    while (temp != NULL) {
        if (game->snake_head->x == temp->x && game->snake_head->y == temp->y && game->snake_head != temp) {
            game->lives -= 1;
            new_snake(game);
            break;
        }
        temp = temp->next;
    }

    printf("%s: %d\t%s: %d\t%s: %d\n", SCORES, game->scores, LIVES, game->lives, SPEED, game->speed);

    for (int line = 0; line < HEIGHT; line++) {
        for (int column = 0; column < WIDTH; column++) {
            temp = game->snake;
            while (temp != NULL) {
                if (line == temp->y && column == temp->x) {
                    if (temp->next == NULL)
                        putchar(SNAKE);
                    else
                        putchar(BODY);
                    break;
                }
                temp = temp->next;
            }
            if (temp) {
                continue;
            }

            if (line == game->apple->y && column == game->apple->x) {
                putchar(APPLE);
            } else if (line == 0 || line == HEIGHT - 1) {
                putchar(WALL1);
            } else if (column == 0 || column == WIDTH - 1) {
                putchar(WALL2);
            } else if (line == MIDSCREENY && column == MIDSCREENX && game->lives <= 0) {
                printf(GAMEOVER);
                column += strlen(GAMEOVER) - 1;
            } else {
                putchar(SPACE);
            }
        }
        putchar(NEWLINE);
    }
}

struct s_apple *init_apple() {
    struct s_apple *apple;
    apple = malloc(sizeof(struct s_apple));
    srand(time(0));
    apple->x = (rand() % (WIDTH - 2)) + 1;
    apple->y = (rand() % (HEIGHT - 2)) + 1;
    return apple;
}

struct s_snake *init_snake() {
    struct s_snake *snake;
    snake = malloc(sizeof(struct s_snake));
    int x = WIDTH / 4;
    int y = HEIGHT - HEIGHT / 4;
    snake->x = x;
    snake->y = y;
    snake->direction = 1;
    snake->next = NULL;
    return snake;
}

struct s_snake *add_snake(struct s_snake *head, int flag) {
    struct s_snake *new;
    new = malloc(sizeof(struct s_snake));
    new->direction = head->direction;
    switch (new->direction) {
        case 1:
            new->x = head->x - 1 * flag;
            new->y = head->y;
            break;
        case 2:
            new->x = head->x;
            new->y = head->y - 1 * flag;
            break;
        case 3:
            new->x = head->x + 1 * flag;
            new->y = head->y;
            break;
        case 4:
            new->x = head->x;
            new->y = head->y + 1 * flag;
            break;
    }
    if (flag == 1) {
        new->next = head;
    } else if (flag == -1) {
        new->next = NULL;
        head->next = new;
    }
    return new;
}

struct s_snake *get_snake_head(struct s_snake *head) {
    struct s_snake *temp = head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    return temp;
}

void new_snake(struct s_game *game) {
    snake_destroy(game->snake);
    game->snake = init_snake();
    add_snake(add_snake(add_snake(game->snake, -1), -1), -1);
    game->snake_head = get_snake_head(game->snake);
}

void move_snake(struct s_snake *snake) {
    struct s_snake *snake_tail = snake;
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
        if (snake_tail->next) snake_tail->direction = snake_tail->next->direction;
        snake_tail = snake_tail->next;
    }
}

void snake_destroy(struct s_snake *head) {
    struct s_snake *temp = head, *next;
    while (temp != NULL) {
        next = temp->next;
        free(temp);
        temp = next;
    }
    next = NULL;
    head = NULL;
}

void apple_destroy(struct s_apple *apple) {
    free(apple);
    apple = NULL;
}
