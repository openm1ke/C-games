#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    struct s_snake *next;
};

struct s_game {
    int scores;
    int lives;
    int speed;
    struct s_apple * apple;
    struct s_snake * snake;
    struct s_snake * snake_head;
};


struct s_snake *draw_garden(struct s_apple *, struct s_snake *, int *, int *);
struct s_snake *init_snake();
void move_snake(struct s_snake *);
struct s_snake *get_snake_head(struct s_snake *);
struct s_apple draw_apple();
struct s_snake *grow_snake(struct s_snake *);
void destroy(struct s_snake *);
struct s_apple * init_apple();
void play(struct s_game *);

struct s_apple * init_apple() {
    struct s_apple * apple;
    apple = malloc(sizeof (struct s_apple));
    srand(time(0));
    apple->x = (rand() % (WIDTH - 2)) + 1;
    apple->y = (rand() % (HEIGHT - 2)) + 1;
    return apple;
}

void snake_destroy(struct s_snake * head) {
    struct s_snake * temp = head, *next;
    while (temp != NULL) {
        next = temp->next;
        free(temp);
        temp = next;
    }
    free(next);
    head = NULL;
}

void apple_destroy(struct s_apple * apple) {
    free(apple);
    apple = NULL;
}

struct s_snake * add_snake(struct s_snake * head, int flag) {
    struct s_snake * new;
    new = malloc(sizeof (struct s_snake));
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
    if(flag == 1) {
        new->next = head;
    } else if (flag == -1) {
        new->next = NULL;
        head->next = new;
    }
    return new;
}


void check(struct s_game * game) {
    struct s_snake * temp;
    add_snake(add_snake(add_snake(game->snake, -1), -1), -1);

    temp = game->snake;
    while(temp != NULL) {
        printf("address = %p, x = %d, y = %d, direction = %d\n", temp, temp->x, temp->y, temp->direction);
        temp = temp->next;
    }

    temp = get_snake_head(game->snake);
    temp->direction = 2;
    move_snake(game->snake);
    printf("Change direction\n");
    temp = game->snake;
    while(temp != NULL) {
        printf("address = %p, x = %d, y = %d, direction = %d\n", temp, temp->x, temp->y, temp->direction);
        temp = temp->next;
    }

    move_snake(game->snake);
    printf("Change direction\n");
    temp = game->snake;
    while(temp != NULL) {
        printf("address = %p, x = %d, y = %d, direction = %d\n", temp, temp->x, temp->y, temp->direction);
        temp = temp->next;
    }


    printf("apple x = %d, apple y = %d\n", game->apple->x, game->apple->y);
    printf("Head of snake - %p, direction = %d\n", get_snake_head(game->snake), game->snake->direction);
    snake_destroy(game->snake);
    game->snake = init_snake();
    add_snake(game->snake, 1);

    temp = game->snake;
    while(temp != NULL) {
        printf("address = %p, x = %d, y = %d, direction = %d\n", temp, temp->x, temp->y, temp->direction);
        temp = temp->next;
    }

    game->lives--;

    printf("Lives %d\n", game->lives);
    apple_destroy(game->apple);
    usleep(1000000);
    game->apple = init_apple();

    printf("apple x = %d, apple y = %d\n", game->apple->x, game->apple->y);

    printf("Head out function = %p, direction = %d\n", game->snake, game->snake->direction);
    struct s_snake * head = get_snake_head(game->snake);
    add_snake(head, 1);

    temp = game->snake;
    while(temp != NULL) {
        printf("address = %p, x = %d, y = %d, direction = %d\n", temp, temp->x, temp->y, temp->direction);
        temp = temp->next;
    }


}

int main() {

    char c = ' ';
    int n = 0;


    struct s_game game;
    game.scores = 0;
    game.lives = 3;
    game.speed = 1000000;
    game.apple = init_apple();
    game.snake = init_snake();

    add_snake(add_snake(add_snake(game.snake, -1), -1), -1);
    //check(&game);

    game.snake_head = get_snake_head(game.snake);



    if (!isatty(STDIN_FILENO)) {
        if (freopen("/dev/tty", "r", stdin) == NULL) {
            printf("Cannot open the file");
        }
    }
    system("stty -icanon");

    while (c != 'q') {
        if (ioctl(0, FIONREAD, &n) == 0 && n > 0) {
            c = getchar();
            if (c == '=') game.speed /= 2;
            if (c == '-') game.speed *= 2;
            if (c == 'd') game.snake_head->direction = 1;
            if (c == 's') game.snake_head->direction = 2;
            if (c == 'a') game.snake_head->direction = 3;
            if (c == 'w') game.snake_head->direction = 4;
            if (c == 'q') break;
        }
        system("clear");

        play(&game);
        //snake = draw_garden(&apple, snake, &scores, &lives);
        //snake_head = get_snake_head(snake);



        if (game.lives <= 0) break;
        usleep(game.speed);
    }


    apple_destroy(game.apple);
    snake_destroy(game.snake);


    return 0;
}

void play(struct s_game * game) {

    struct s_snake * temp;

    move_snake( game->snake);

    if (game->snake_head->x == game->apple->x && game->snake_head->y == game->apple->y) {
        apple_destroy(game->apple);
        game->apple = init_apple();
        game->scores += 10;
        game->snake = add_snake(game->snake, 1);
    }

    if (game->snake_head->x == WIDTH - 1 || game->snake_head->x == 0||
        game->snake_head->y == HEIGHT - 1 || game->snake_head->y == 0) {
        game->lives -= 1;
        snake_destroy(game->snake);
        game->snake = init_snake();
        add_snake(add_snake(add_snake(game->snake, -1), -1), -1);
        game->snake_head = get_snake_head(game->snake);
    }

    temp = game->snake;
    while (temp != NULL) {
        if (game->snake_head->x == temp->x && game->snake_head->y == temp->y && game->snake_head != temp) {
            game->lives -= 1;
            snake_destroy(game->snake);
            game->snake = init_snake();
            add_snake(add_snake(add_snake(game->snake, -1), -1), -1);
            game->snake_head = get_snake_head(game->snake);
            break;
        }
        temp = temp->next;
    }

    printf("%s: %d\t%s: %d\n", SCORES, game->scores, LIVES, game->lives);

    for (int line = 0; line < HEIGHT; line++) {
        for (int column = 0; column < WIDTH; column++) {
            temp = game->snake;
            while (temp != NULL) {
                if (line == temp->y && column == temp->x) {
                    putchar(SNAKE);
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

struct s_snake *get_snake_head(struct s_snake *head) {
    struct s_snake *temp = head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    return temp;
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

struct s_snake *init_snake() {
    struct s_snake * snake;
    snake = malloc(sizeof(struct s_snake));
    int d = 1;
    int x = WIDTH / 4;
    int y = HEIGHT - HEIGHT / 4;
    snake->x = x;
    snake->y = y;
    snake->direction = d;
    snake->next = NULL;
    return snake;
}

struct s_apple draw_apple() {
    struct s_apple apple;
    srand(time(0));
    apple.x = (rand() % (WIDTH - 2)) + 1;
    apple.y = (rand() % (HEIGHT - 2)) + 1;
    return apple;
}

struct s_snake *grow_snake(struct s_snake *snake) {
    struct s_snake *new_chain = malloc(sizeof (struct s_snake));
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

void destroy(struct s_snake * snake) {
    free(snake);
}

struct s_snake *draw_garden(struct s_apple *apple, struct s_snake *snake, int *scores, int *lives) {
    struct s_snake *tmp = snake;
    struct s_snake *snake_head = get_snake_head(tmp);

    if (snake_head->x == apple->x && snake_head->y == apple->y) {
        *apple = draw_apple();
        *scores += 10;
        tmp = grow_snake(snake);
    }

    if (snake_head->x == WIDTH - 1 || snake_head->x == 0 || snake_head->y == HEIGHT - 1 ||
        snake_head->y == 0) {
        *lives -= 1;
        destroy(tmp);
        tmp = init_snake();
    }

    struct s_snake * temp = tmp;
    while (temp != NULL) {
        if (snake_head->x == temp->x && snake_head->y == temp->y && snake_head != temp) {
            tmp = init_snake();
            *lives -= 1;
            destroy(snake);
            break;
        }
        temp = temp->next;
    }

    printf("%s: %d\t%s: %d\n", SCORES, *scores, LIVES, *lives);

    for (int line = 0; line < HEIGHT; line++) {
        for (int column = 0; column < WIDTH; column++) {
            temp = tmp;
            while (temp != NULL) {
                if (line == temp->y && column == temp->x) {
                    putchar(SNAKE);
                    break;
                }
                temp = temp->next;
            }
            if (temp) {
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

    return tmp;
}
