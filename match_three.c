#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#define WIDTH 13
#define HEIGHT 12
#define WALL1 '-'
#define WALL2 '|'
#define SPACE ' '
#define NEWLINE '\n'
#define GAMEOVER "Game over!"
#define MIDSCREENY 2
#define MIDSCREENX HEIGHT / 2
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
    int **board;
    struct s_block *block;
};

void draw_board(struct s_game *game);
void block_swap_values(struct s_game *game);
void block_check_collision(struct s_game *game, int step);
void board_save_values(struct s_game *game);
int **init_temp_board(struct s_game *game);
int count_board_values(int **board);
void temp_delete_zeros(int **temp);
int check_board_diff(int ** board1, int ** board2);

struct s_block *init_block();
int **init_board();
void destroy_block(struct s_block *);
void destroy_board(int **);

int main() {
    struct s_game game;
    char c = ' ';
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
        d = 2;  //  drop down everytime
        if (ioctl(0, FIONREAD, &n) == 0 && n > 0) {
            c = getchar();
            if (c == '=' || c == '+') game.speed -= SPEED_INC;
            if (c == '-') game.speed += SPEED_INC;
            if (c == ' ')
                d = 0;
            if (c == 'd') d = 1;
            if (c == 's') {
                d = 2;
                game.speed = SPEED_INC;
            }
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

void destroy_block(struct s_block *block) { free(block); }

void destroy_board(int **board) {
    for (int i = 0; i < HEIGHT + 1; i++) {
        free(board[i]);
    }
    free(board);
}

int **init_board() {
    int **pointer_array = malloc((HEIGHT + 1) * sizeof(int *));
    for (int i = 0; i < HEIGHT + 1; i++) {
        pointer_array[i] = malloc((WIDTH + 2) * sizeof(int));
    }
    return pointer_array;
}

struct s_block *init_block() {
    struct s_block *block;
    srand(time(0));
    block = malloc(sizeof(struct s_block));
    block->y = WIDTH / 2;
    block->direction = rand() % 2;
    if (block->direction == 1)
        block->x = -1;
    else
        block->x = 0;
    block->v1 = rand() % 4 + 1;
    block->v2 = rand() % 4 + 1;
    block->v3 = rand() % 4 + 1;
    return block;
}

void block_check_collision(struct s_game *game, int step) {

    if(game->is_over == 1 || game->block == NULL) return;

    int d = game->block->direction;
    switch (step) {
        case 0:
            if (d == 0 &&
                game->block->x - 1 >= 0 &&
                game->block->x + 1 != HEIGHT &&
                game->board[game->block->x + 1][game->block->y] == 0 &&
                game->board[game->block->x - 1][game->block->y] == 0)
                    game->block->direction = 1 - game->block->direction;
            if (d == 1 && game->block->x >= 0 &&
                game->block->y - 1 != 0 &&
                game->block->y + 1 != WIDTH &&
                game->board[game->block->x][game->block->y + 1] == 0 &&
                game->board[game->block->x][game->block->y - 1] == 0)
                    game->block->direction = 1 - game->block->direction;
            break;
        case 1:
            if (game->block->x - 1 < 0) break;
            if (d == 0 &&
               (game->block->y + 2 >= WIDTH ||
                game->board[game->block->x][game->block->y + 2 ] != 0))
                    break;
            if (d == 1 &&
                    (game->block->y + 1 == WIDTH ||
                game->board[game->block->x][game->block->y + 1] != 0 ||
                game->board[game->block->x - 1][game->block->y + 1] != 0 ||
                game->board[game->block->x + 1][game->block->y + 1] != 0))
                    break;
            if ((d == 0 && game->board[game->block->x][game->block->y + 2] == 0) ||
                (d == 1 &&
                game->board[game->block->x][game->block->y + 1] == 0 &&
                game->board[game->block->x + 1][game->block->y + 1] == 0 &&
                game->board[game->block->x - 1][game->block->y + 1] == 0))
                    game->block->y++;
            break;
        case 2:
            if ((d == 1 &&
                 game->board[game->block->x + 2][game->block->y] == 0 &&
                 game->block->x + 2 != HEIGHT) ||
                (d == 0 && game->board[game->block->x + 1][game->block->y] == 0 &&
                 game->board[game->block->x + 1][game->block->y + 1] == 0 &&
                 game->board[game->block->x + 1][game->block->y - 1] == 0 &&
                 game->block->x + 1 != HEIGHT)) {
                    game->block->x++;
            } else {
                if (d == 1 && game->block->x <= 0) {
                    game->is_over = 1;
                    return;
                }
                if (d == 0 && game->block->x == 0 && (game->board[game->block->x + 1][game->block->y + 1] > 0 ||
                                      game->board[game->block->x + 1][game->block->y - 1] > 0 ||
                                      game->board[game->block->x + 1][game->block->y] > 0)) {
                    game->is_over = 1;
                    return;
                }
                board_save_values(game);
                game->speed = SPEED_START;
            }
            break;
        case 3:
            if (game->block->x - 1 < 0) break;
            if (d == 0 &&
                (game->block->y - 2 <= 0 ||
                 game->board[game->block->x][game->block->y - 2 ] != 0))
                break;
            if (d == 1 &&
                (game->block->y - 1 == 0 ||
                 game->board[game->block->x][game->block->y - 1] != 0 ||
                 game->board[game->block->x - 1][game->block->y - 1] != 0 ||
                 game->board[game->block->x + 1][game->block->y - 1] != 0))
                break;
            if ((d == 0 && game->board[game->block->x][game->block->y - 2] == 0) ||
                (d == 1 &&
                 game->board[game->block->x][game->block->y - 1] == 0 &&
                 game->board[game->block->x + 1][game->block->y - 1] == 0 &&
                 game->board[game->block->x - 1][game->block->y - 1] == 0))
                game->block->y--;
            break;
        default:
            break;
    }
}

void board_save_values(struct s_game *game) {

    if (game->block->direction == 1 && game->block->x - 1 < 0) {
        game->is_over = 1;
        return;
    }

    if (game->block->direction == 1) {
        game->board[game->block->x - 1][game->block->y] = game->block->v1;
        game->board[game->block->x][game->block->y] = game->block->v2;
        game->board[game->block->x + 1][game->block->y] = game->block->v3;
    } else if (game->block->direction == 0) {
        game->board[game->block->x][game->block->y - 1] = game->block->v1;
        game->board[game->block->x][game->block->y] = game->block->v2;
        game->board[game->block->x][game->block->y + 1] = game->block->v3;
    }

    int count_board;
    int count_temp;
    int **temp;
    int diff;
    do {
        temp_delete_zeros(game->board);
        count_board = count_board_values(game->board);
        temp = init_temp_board(game);
        count_temp = count_board_values(temp);
        temp_delete_zeros(temp);
        game->scores += (count_board - count_temp) * 10;
        diff = check_board_diff(game->board, temp);
        destroy_board(game->board);
        game->board = temp;
    } while (count_temp < count_board || diff == 1);
    destroy_block(game->block);
    game->block = init_block();
    if ((game->block->direction == 1 && (game->board[game->block->x + 1][game->block->y] > 0 ||
                                         game->board[game->block->x + 2][game->block->y] > 0)) ||
        (game->block->direction == 0 && (game->board[game->block->x][game->block->y + 1] > 0 ||
                                         game->board[game->block->x][game->block->y - 1] > 0 ||
                                         game->board[game->block->x][game->block->y] > 0))) {
        game->is_over = 1;
        destroy_block(game->block);
        game->block = NULL;
    }
}
int check_board_diff(int ** board1, int ** board2) {
    int diff = 0;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH + 1; j++) {
            if (board1[i][j] != board2[i][j]) diff = 1;
        }
    }
    return diff;
}
void temp_delete_zeros(int **temp) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH + 1; j++) {
            if (i + 1 != HEIGHT) {
                if (temp[i][j] != 0 && temp[i + 1][j] == 0) {
                    temp[i + 1][j] = temp[i][j];
                    temp[i][j] = 0;
                }
            }
        }
    }
}

int count_board_values(int **board) {
    int count = 0;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH + 1; j++) {
            if (board[i][j] > 0) count++;
        }
    }
    return count;
}

int **init_temp_board(struct s_game *game) {
    int **temp = init_board();
    int temp_value;
    int flag;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH + 1; j++) {
            temp_value = game->board[i][j];
            temp[i][j] = temp_value;
            flag = 0;
            if (temp_value == game->board[i][j - 1] && temp_value == game->board[i][j + 1]) {
                temp[i][j] = 0;
                temp[i][j - 1] = 0;
                temp[i][j + 1] = 0;
                game->board[i][j + 1] = 0;

                if (j + 2 <= WIDTH + 1 && temp_value == game->board[i][j + 2]) {
                    temp[i][j + 2] = 0;
                    game->board[i][j + 2] = 0;
                    flag = 1;
                }
                if (j + 3 <= WIDTH + 1 && temp_value == game->board[i][j + 3] && flag == 1) {
                    temp[i][j + 3] = 0;
                    game->board[i][j + 3] = 0;
                    flag = 1;
                }
                if (j + 4 <= WIDTH + 1 && temp_value == game->board[i][j + 4] && flag == 1) {
                    temp[i][j + 4] = 0;
                    game->board[i][j + 4] = 0;
                }
                if (temp_value == game->board[i][j - 2]) {
                    temp[i][j - 2] = 0;
                    game->board[i][j - 2] = 0;
                }
            }
            flag = 0;
            if ((i - 1 >= 0 && i + 1 < HEIGHT) && temp_value == game->board[i - 1][j] && temp_value == game->board[i + 1][j] && temp_value != 0) {
                temp[i][j] = 0;
                temp[i - 1][j] = 0;
                temp[i + 1][j] = 0;
                game->board[i + 1][j] = 0;

                if (i + 2 <= HEIGHT && temp_value == game->board[i + 2][j]) {
                    temp[i + 2][j] = 0;
                    game->board[i + 2][j] = 0;
                    flag = 1;
                }
                if (i + 3 <= HEIGHT && temp_value == game->board[i + 3][j] && flag == 1) {
                    temp[i + 3][j] = 0;
                    game->board[i + 3][j] = 0;
                }
            }
        }
    }
    return temp;
}

void block_swap_values(struct s_game *game) {
    int temp;
    temp = game->block->v1;
    game->block->v1 = game->block->v2;
    game->block->v2 = game->block->v3;
    game->block->v3 = temp;
}

void draw_board(struct s_game *game) {

    int block1x = -1, block1y = -1, block2x = -1, block2y = -1, block3x = -1, block3y= -1;
    if (game->is_over != 1 && game->block != NULL) {
        switch (game->block->direction) {
            case 1:
                block1x = game->block->x - 1;
                block2x = game->block->x;
                block3x = game->block->x + 1;
                block1y = game->block->y;
                block2y = game->block->y;
                block3y = game->block->y;
                break;
            case 0:
                block1y = game->block->y - 1;
                block2y = game->block->y;
                block3y = game->block->y + 1;
                block1x = game->block->x;
                block2x = game->block->x;
                block3x = game->block->x;
                break;
        }
    }

    for (int i = 0; i < HEIGHT + 1; i++) {
        for (int j = 0; j < WIDTH + 1; j++) {

            if (i + 1== HEIGHT + 1) {
                putchar(WALL1);
            } else if (j == 0 || j + 2 == WIDTH + 2) {
                putchar(WALL2);
            } else if (j == MIDSCREENY && i == MIDSCREENX && game->is_over == 1) {
                printf(GAMEOVER);
                j += strlen(GAMEOVER) - 1;
            } else if (i == block1x && j == block1y) {
                    printf("%d", game->block->v1);
            } else if (i == block2x && j == block2y) {
                    printf("%d", game->block->v2);
            } else if (i == block3x && j == block3y) {
                    printf("%d", game->block->v3);
            } else if (game->board[i][j] > 0) {
                printf("%d", game->board[i][j]);
            } else {
                putchar(SPACE);
            }
        }
        putchar(NEWLINE);
    }
    printf("Scores: %d\n", game->scores);
}
