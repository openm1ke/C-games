#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#define WIDTH 16
#define HEIGHT 17
#define WALL1 '-'
#define WALL2 '|'
#define SPACE ' '
#define NEWLINE '\n'
#define GAMEOVER "Game over!"
#define MIDSCREENX (WIDTH / 2) - strlen(GAMEOVER) / 2
#define MIDSCREENY HEIGHT / 2
#define SPEED_START 500000
#define SPEED_INC 5000

struct s_block {
    int x;
    int y;
    int v1;
    int v2;
    int v3;
    int direction;
};

struct s_game {
    int scores;
    int speed;
    int is_over;
    int ** board;
    struct s_block * block;
};

void draw_board(struct s_game * game);
void block_swap_values(struct s_game * game);
void block_check_collision(struct s_game * game, int step);
void board_save_values(struct s_game * game);
int ** init_temp_board(struct s_game * game);
int count_board_values(int ** board);
void temp_delete_zeros(int ** temp);

struct s_block * init_block();
int ** init_board();
void destroy_block(struct s_block *);
void destroy_board(int **);

int main() {

    struct s_game game;
    char c;
    int n = 0;
    int d;

    game.scores = 0;
    game.is_over = 0;
    game.speed = SPEED_START;
    game.board = init_board();
    game.block = init_block();

    if (!isatty(STDIN_FILENO)) {
        if (freopen("/dev/tty", "r", stdin) == NULL) {
            printf("Cannot open the file");
        }
    }
    system("stty -icanon");

    while (c != 'q') {
        d = 2; //  drop down everytime
        if (ioctl(0, FIONREAD, &n) == 0 && n > 0) {
            c = getchar();
            if (c == '=' || c == '+') game.speed -= SPEED_INC;
            if (c == '-') game.speed += SPEED_INC;
            if (c == ' ') d = 0; //  game.block->direction = 1 - game.block->direction; //  flip between 0 and 1
            if (c == 'd') d = 1;
            if (c == 's') { d = 2; game.speed = SPEED_INC; }
            if (c == 'a') d = 3;
            if (c == 'w') block_swap_values(&game);
            if (c == 'q') break;
        }
        system("clear");
        block_check_collision(&game, d);
        draw_board(&game);
        if (game.is_over == 1) break;
        usleep(game.speed);
    }
    destroy_block(game.block);
    destroy_board(game.board);
    return 0;
}

void destroy_block(struct s_block * block) {
    free(block);
}

void destroy_board(int ** board) {
    for (int i = 0; i < WIDTH; i++) {
        free(board[i]);
    }
    free(board);
}

int ** init_board() {
    int **pointer_array = malloc(HEIGHT * sizeof(int *));
    for (int i = 0; i < WIDTH; i++) {
        pointer_array[i] = malloc(WIDTH * sizeof(int));
    }
    return pointer_array;
}

struct s_block * init_block() {
    struct s_block * block;
    srand(time(0));
    block = malloc(sizeof(struct s_block));
    block->x = WIDTH / 2;
    block->y = 1;
    block->direction = rand() % 2;
    block->v1 = rand() % 4 + 1;
    block->v2 = rand() % 4 + 1;
    block->v3 = rand() % 4 + 1;
    return block;
}


void block_check_collision(struct s_game * game, int step) {
    int d;
    switch (step) {
        case 0:
            d = 1 - game->block->direction;
            if (d == 0 && game->board[game->block->x + 1][game->block->y] == 0 &&
                game->block->x + 1 != WIDTH - 1 &&
                game->board[game->block->x - 1][game->block->y] == 0 &&
                game->block->x - 1 != 0)
                    game->block->direction = 1 - game->block->direction;
            if (d == 1 && game->board[game->block->x][game->block->y + 1] == 0 &&
                game->board[game->block->x][game->block->y - 1] == 0 &&
                game->block->y + 1 != HEIGHT - 1)
                    game->block->direction = 1 - game->block->direction;
            //printf("%d\n", d);
            break;
        case 1:
            if ((game->block->direction == 1 && game->block->x + 1 == WIDTH - 1) ||
                (game->block->direction == 0 && (game->block->x + 2 == WIDTH - 1 ||
                game->board[game->block->x + 2][game->block->y] != 0)))
                    break;
            if (game->board[game->block->x + 1][game->block->y] == 0 &&
                game->board[game->block->x + 1][game->block->y - 1] == 0 &&
                game->board[game->block->x + 1][game->block->y + 1] == 0)
                    game->block->x++;
            break;
        case 2:
            if ((game->block->direction == 1 &&
                 game->board[game->block->x][game->block->y + 2] == 0 &&
                 game->block->y + 2 != HEIGHT - 1) || (game->block->direction == 0 &&
                game->board[game->block->x][game->block->y + 1] == 0 &&
                game->board[game->block->x - 1][game->block->y + 1] == 0 &&
                game->board[game->block->x + 1][game->block->y + 1] == 0 &&
                game->block->y + 1 != HEIGHT - 1)) {
                game->block->y++;
            } else {
                //printf("Stopped\n");
                board_save_values(game); //  если нет возможности двигаться, то сохраняем значения и создаем новый блок
                game->speed = SPEED_START;
            }
            break;
        case 3:
            if ((game->block->direction == 1 && game->block->x - 1 == 0) ||
                (game->block->direction == 0 && (game->block->x - 2 == 0 ||
                game->board[game->block->x - 2][game->block->y] != 0)))
                break;
            if (game->board[game->block->x - 1][game->block->y] == 0 &&
                game->board[game->block->x - 1][game->block->y - 1] == 0 &&
                game->board[game->block->x - 1][game->block->y + 1] == 0)
                    game->block->x--;
            break;
        default:
            break;
    }
}

void board_save_values(struct s_game * game) {
    if (game->block->direction == 0) {
        game->board[game->block->x - 1][game->block->y] = game->block->v1;
        game->board[game->block->x][game->block->y] = game->block->v2;
        game->board[game->block->x + 1][game->block->y] = game->block->v3;
    } else {
        game->board[game->block->x][game->block->y - 1] = game->block->v1;
        game->board[game->block->x][game->block->y] = game->block->v2;
        game->board[game->block->x][game->block->y + 1] = game->block->v3;
    }

    int count_board;
    int count_temp;
    int **temp;

    do {
        count_board = count_board_values(game->board);
        temp = init_temp_board(game);
        count_temp = count_board_values(temp);
        temp_delete_zeros(temp);
        destroy_board(game->board);
        game->board = temp;
    } while (count_temp < count_board);

    destroy_block(game->block);
    game->block = init_block();

    if ((game->block->direction == 1 &&
        (game->board[game->block->x][game->block->y] > 0 ||
         game->board[game->block->x][game->block->y+1] > 0)) ||
        (game->block->direction == 0 &&
        (game->board[game->block->x][game->block->y] > 0 ||
         game->board[game->block->x+1][game->block->y] > 0 ||
         game->board[game->block->x-1][game->block->y] > 0)))
            game->is_over = 1;
    if(game->is_over == 1) return;
}

void temp_delete_zeros(int ** temp) {
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            if (temp[i][j] != 0 && temp[i][j + 1] == 0 && j + 1 != HEIGHT - 1) {
                temp[i][j + 1] = temp[i][j];
                temp[i][j] = 0;
            }
        }
    }

    for (int i = WIDTH - 1; i > 0; i--) {
        for (int j = HEIGHT - 2; j > 0; j--) {
            if (temp[i][j] == 0 && temp[i][j - 1] != 0) {
                temp[i][j] = temp[i][j - 1];
                temp[i][j - 1] = 0;
            }
        }
    }
}

int count_board_values(int ** board) {
    int count = 0;
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            if (board[i][j] > 0) count++;
        }
    }
    return count;
}

int ** init_temp_board(struct s_game * game) {
    int ** temp = init_board();
    int temp_value;

    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            temp_value = game->board[i][j];
            temp[i][j] = temp_value;
            if (temp_value == game->board[i][j - 1] && temp_value == game->board[i][j + 1]) {
                temp[i][j] = 0;
                temp[i][j - 1] = 0;
                temp[i][j + 1] = 0;
                game->board[i][j+1] = 0;
                if (j + 2 != HEIGHT && temp_value == game->board[i][j + 2]) {
                    temp[i][j + 2] = 0;
                    game->board[i][j + 2] = 0;
                }
                if (j + 3 != HEIGHT && temp_value == game->board[i][j + 3]) {
                    temp[i][j + 3] = 0;
                    game->board[i][j + 3] = 0;
                }
                if (temp_value == game->board[i][j - 2]) { temp[i][j-2] = 0; game->board[i][j-2] = 0; }
            } else if (temp_value == game->board[i - 1][j] && temp_value == game->board[i + 1][j]) {
                temp[i][j] = 0;
                temp[i - 1][j] = 0;
                temp[i + 1][j] = 0;
                game->board[i+1][j] = 0;
                if (i + 2 < WIDTH && temp_value == game->board[i + 2][j]) {
                    temp[i + 2][j] = 0;
                    game->board[i + 2][j] = 0;
                }
                if (i - 2 > 0) {
                    if (temp_value == game->board[i - 2][j]) {
                        temp[i - 2][j] = 0;
                        game->board[i - 2][j] = 0;
                    }
                }
            }
        }
    }
    return temp;
}

void block_swap_values(struct s_game * game) {
    int temp;
    temp = game->block->v1;
    game->block->v1 = game->block->v2;
    game->block->v2 = game->block->v3;
    game->block->v3 = temp;
}

void draw_board(struct s_game * game) {
    int block1x, block1y, block2x, block2y, block3x, block3y;
    if(game->is_over != 1) {
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
    }

    for (int line = 0; line < HEIGHT; line++) {
        for (int column = 0; column < WIDTH; column++) {
            if (line == HEIGHT - 1) {
                putchar(WALL1);
            } else if (column == 0 || column == WIDTH - 1) {
                putchar(WALL2);
            } else if (line == MIDSCREENY && column == MIDSCREENX && game->is_over == 1) {
                printf(GAMEOVER);
                column += strlen(GAMEOVER) - 1;
            } else if (line == block1y && column == block1x) {
                printf("%d", game->block->v1);
            } else if (line == block2y && column == block2x) {
                printf("%d", game->block->v2);
            } else if (line == block3y && column == block3x) {
                printf("%d", game->block->v3);
            } else if (line > 0 && column > 0 && game->board[column][line] > 0) {
                printf("%d", game->board[column][line]);
            } else {
                putchar(SPACE);
            }
        }
        putchar(NEWLINE);
    }
}
