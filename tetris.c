#define _XOPEN_SOURCE 500
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define NUM_PIECES 7
#define NUM_ROTATIONS 4
#define MATRIX_SIZE 4

#define FIELD_WIDTH 10
#define FIELD_HEIGHT 20
#define CELL_WIDTH 2
#define BOTTOM_ROW 24

#define STATS_ROW 8
#define STATS_COL 1

#define NEXT_PIECE_ROW 13
#define NEXT_PIECE_COL 1

#define FIELD_ROW 1
#define FIELD_COL 15

#define HINTS_ROW 8
#define HINTS_COL 42

char tetromino[NUM_PIECES][NUM_ROTATIONS][MATRIX_SIZE][MATRIX_SIZE] = {
    {{{'.', '.', '.', '.'}, {'X', 'X', 'X', 'X'}, {'.', '.', '.', '.'}, {'.', '.', '.', '.'}},
     {{'.', '.', 'X', '.'}, {'.', '.', 'X', '.'}, {'.', '.', 'X', '.'}, {'.', '.', 'X', '.'}},
     {{'.', '.', '.', '.'}, {'.', '.', '.', '.'}, {'X', 'X', 'X', 'X'}, {'.', '.', '.', '.'}},
     {{'.', 'X', '.', '.'}, {'.', 'X', '.', '.'}, {'.', 'X', '.', '.'}, {'.', 'X', '.', '.'}}},

    {{{'.', '.', '.', '.'}, {'.', 'X', 'X', 'X'}, {'.', '.', 'X', '.'}, {'.', '.', '.', '.'}},
     {{'.', '.', '.', '.'}, {'.', '.', 'X', '.'}, {'.', 'X', 'X', '.'}, {'.', '.', 'X', '.'}},
     {{'.', '.', '.', '.'}, {'.', 'X', '.', '.'}, {'X', 'X', 'X', '.'}, {'.', '.', '.', '.'}},
     {{'.', 'X', '.', '.'}, {'.', 'X', 'X', '.'}, {'.', 'X', '.', '.'}, {'.', '.', '.', '.'}}},

    {{{'.', '.', '.', '.'}, {'.', 'X', 'X', '.'}, {'.', 'X', 'X', '.'}, {'.', '.', '.', '.'}},

     {{'.', '.', '.', '.'}, {'.', 'X', 'X', '.'}, {'.', 'X', 'X', '.'}, {'.', '.', '.', '.'}},
     {{'.', '.', '.', '.'}, {'.', 'X', 'X', '.'}, {'.', 'X', 'X', '.'}, {'.', '.', '.', '.'}},
     {{'.', '.', '.', '.'}, {'.', 'X', 'X', '.'}, {'.', 'X', 'X', '.'}, {'.', '.', '.', '.'}}},

    {{{'.', 'X', 'X', '.'}, {'.', '.', 'X', '.'}, {'.', '.', 'X', '.'}, {'.', '.', '.', '.'}},
     {{'.', '.', '.', '.'}, {'.', '.', 'X', '.'}, {'X', 'X', 'X', '.'}, {'.', '.', '.', '.'}},
     {{'.', 'X', '.', '.'}, {'.', 'X', '.', '.'}, {'.', 'X', 'X', '.'}, {'.', '.', '.', '.'}},
     {{'.', '.', '.', '.'}, {'.', 'X', 'X', 'X'}, {'.', 'X', '.', '.'}, {'.', '.', '.', '.'}}},

    {{{'.', 'X', 'X', '.'}, {'.', 'X', '.', '.'}, {'.', 'X', '.', '.'}, {'.', '.', '.', '.'}},
     {{'.', '.', '.', '.'}, {'X', 'X', 'X', '.'}, {'.', '.', 'X', '.'}, {'.', '.', '.', '.'}},
     {{'.', '.', 'X', '.'}, {'.', '.', 'X', '.'}, {'.', 'X', 'X', '.'}, {'.', '.', '.', '.'}},
     {{'.', '.', '.', '.'}, {'.', 'X', '.', '.'}, {'.', 'X', 'X', 'X'}, {'.', '.', '.', '.'}}},

    {{{'.', '.', '.', '.'}, {'.', 'X', 'X', '.'}, {'X', 'X', '.', '.'}, {'.', '.', '.', '.'}},
     {{'.', 'X', '.', '.'}, {'.', 'X', 'X', '.'}, {'.', '.', 'X', '.'}, {'.', '.', '.', '.'}},
     {{'.', '.', '.', '.'}, {'.', '.', 'X', 'X'}, {'.', 'X', 'X', '.'}, {'.', '.', '.', '.'}},
     {{'.', '.', '.', '.'}, {'.', 'X', '.', '.'}, {'.', 'X', 'X', '.'}, {'.', '.', 'X', '.'}}},

    {{{'.', '.', '.', '.'}, {'X', 'X', '.', '.'}, {'.', 'X', 'X', '.'}, {'.', '.', '.', '.'}},
     {{'.', '.', 'X', '.'}, {'.', 'X', 'X', '.'}, {'.', 'X', '.', '.'}, {'.', '.', '.', '.'}},
     {{'.', '.', '.', '.'}, {'.', 'X', 'X', '.'}, {'.', '.', 'X', 'X'}, {'.', '.', '.', '.'}},
     {{'.', '.', '.', '.'}, {'.', '.', 'X', '.'}, {'.', 'X', 'X', '.'}, {'.', 'X', '.', '.'}}}};

int field[FIELD_HEIGHT][FIELD_WIDTH];

int falling_piece = 1;
int falling_rotation = 0;
int falling_x = 3;
int falling_y = 0;

typedef struct {
    int field[FIELD_HEIGHT][FIELD_WIDTH];
    int score;
    int lines;
    int level;
    int currentPiece;
    int currentRotation;
    int currentX;
    int currentY;
    int nextPiece;
    int nextRotation;
    int gameOver;
} GameState;

void clearScreen() { printf("\033[H\033[J"); }

void moveCursor(int row, int col) { printf("\033[%d;%dH", row, col); }

void setCursorDown() {
    moveCursor(BOTTOM_ROW, 0);
    printf("\033[?25h");
}

void hideCursor() { printf("\033[?25l"); }

void showCursor() { printf("\033[?25h"); }

void drawStats(GameState *state) {
    moveCursor(STATS_ROW, STATS_COL);
    printf("Lines: %d", state->lines);
    moveCursor(STATS_ROW + 1, STATS_COL);
    printf("Level: %d", state->level);
    moveCursor(STATS_ROW + 2, STATS_COL);
    printf("Score: %d", state->score);
}

void drawNextPiece(GameState *state) {
    int piece = state->nextPiece;
    int rotation = state->nextRotation;
    moveCursor(NEXT_PIECE_ROW, NEXT_PIECE_COL);
    printf("Next Piece:");
    for (int i = 0; i < MATRIX_SIZE; i++) {
        moveCursor(NEXT_PIECE_ROW + i + 1, NEXT_PIECE_COL);
        for (int j = 0; j < MATRIX_SIZE; j++) {
            char c = tetromino[piece][rotation][i][j];
            if (c == 'X')
                printf("[]");
            else
                printf("  ");
        }
    }
}

void drawField(GameState *state) {
    moveCursor(FIELD_ROW, FIELD_COL);
    printf("<!");
    for (int i = 0; i < FIELD_WIDTH * CELL_WIDTH; i++) {
        printf(" ");
    }
    printf("!>");

    for (int y = 0; y < FIELD_HEIGHT; y++) {
        moveCursor(FIELD_ROW + 1 + y, FIELD_COL);
        printf("<!");
        for (int x = 0; x < FIELD_WIDTH; x++) {
            if (state->field[y][x] == 0)
                printf("  ");
            else
                printf("[]");
        }
        printf("!>");
    }

    int bottomRow = FIELD_ROW + 1 + FIELD_HEIGHT;
    moveCursor(bottomRow, FIELD_COL);
    printf("<!");
    for (int i = 0; i < FIELD_WIDTH * CELL_WIDTH; i++) {
        printf("=");
    }
    printf("!>");

    moveCursor(bottomRow + 1, FIELD_COL + 2);
    for (int i = 0; i < FIELD_WIDTH; i++) {
        printf("\\/");
    }
}

void drawHints() {
    moveCursor(HINTS_ROW, HINTS_COL);
    printf("Controls:");
    moveCursor(HINTS_ROW + 1, HINTS_COL);
    printf("A - Left");
    moveCursor(HINTS_ROW + 2, HINTS_COL);
    printf("D - Right");
    moveCursor(HINTS_ROW + 3, HINTS_COL);
    printf("W - Rotate");
    moveCursor(HINTS_ROW + 4, HINTS_COL);
    printf("S - Down");
    moveCursor(HINTS_ROW + 5, HINTS_COL);
    printf("Space - Drop");
}

void drawFallingPiece(GameState *state) {
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            if (tetromino[state->currentPiece][state->currentRotation][i][j] == 'X') {
                int screen_row = FIELD_ROW + 1 + state->currentY + i;
                int screen_col = FIELD_COL + 2 + (state->currentX + j) * CELL_WIDTH;
                moveCursor(screen_row, screen_col);
                printf("[]");
            }
        }
    }
}

int collision(GameState *state, int x, int y, int piece, int rotation) {
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            if (tetromino[piece][rotation][i][j] == 'X') {
                int fx = x + j;
                int fy = y + i;
                if (fx < 0 || fx >= FIELD_WIDTH || fy < 0 || fy >= FIELD_HEIGHT) return 1;
                if (state->field[fy][fx] != 0) return 1;
            }
        }
    }
    return 0;
}

void lockPiece(GameState *state) {
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            if (tetromino[state->currentPiece][state->currentRotation][i][j] == 'X') {
                int fx = state->currentX + j;
                int fy = state->currentY + i;
                if (fx >= 0 && fx < FIELD_WIDTH && fy >= 0 && fy < FIELD_HEIGHT) {
                    state->field[fy][fx] = 1;
                }
            }
        }
    }
}

int clearLines(GameState *state) {
    int linesCleared = 0;
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        int full = 1;
        for (int x = 0; x < FIELD_WIDTH; x++) {
            if (state->field[y][x] == 0) {
                full = 0;
                break;
            }
        }
        if (full) {
            linesCleared++;
            for (int ty = y; ty > 0; ty--) {
                for (int x = 0; x < FIELD_WIDTH; x++) {
                    state->field[ty][x] = state->field[ty - 1][x];
                }
            }
            for (int x = 0; x < FIELD_WIDTH; x++) {
                state->field[0][x] = 0;
            }
        }
    }
    return linesCleared;
}

void updateScore(GameState *state, int linesCleared) {
    state->lines += linesCleared;
    state->score += linesCleared * 100;
    if (state->lines / 10 > state->level) state->level = state->lines / 10;
}

int generateRandomPiece(void) { return rand() % NUM_PIECES; }

void processInput(GameState *state) {
    int ch;

    while ((ch = getchar()) != EOF) {
        switch (ch) {
            case 'a':
            case 'A':
                if (!collision(state, state->currentX - 1, state->currentY, state->currentPiece,
                               state->currentRotation))
                    state->currentX--;
                break;
            case 'd':
            case 'D':
                if (!collision(state, state->currentX + 1, state->currentY, state->currentPiece,
                               state->currentRotation))
                    state->currentX++;
                break;
            case 'w':
            case 'W': {
                int newRotation = (state->currentRotation + 1) % NUM_ROTATIONS;
                if (!collision(state, state->currentX, state->currentY, state->currentPiece, newRotation))
                    state->currentRotation = newRotation;
                break;
            }
            case 's':
            case 'S':
                if (!collision(state, state->currentX, state->currentY + 1, state->currentPiece,
                               state->currentRotation))
                    state->currentY++;
                break;
            case ' ':

                while (!collision(state, state->currentX, state->currentY + 1, state->currentPiece,
                                  state->currentRotation)) {
                    state->currentY++;
                }
                break;
            default:
                break;
        }
    }
}

void gameLoop(GameState *state) {
    const useconds_t gameSpeed = 500000;
    while (!state->gameOver) {
        processInput(state);

        if (!collision(state, state->currentX, state->currentY + 1, state->currentPiece,
                       state->currentRotation)) {
            state->currentY++;
        } else {
            lockPiece(state);
            int linesCleared = clearLines(state);
            updateScore(state, linesCleared);

            state->currentPiece = state->nextPiece;
            state->currentRotation = state->nextRotation;
            state->currentX = FIELD_WIDTH / 2 - MATRIX_SIZE / 2;
            state->currentY = 0;

            state->nextPiece = generateRandomPiece();
            state->nextRotation = 0;

            if (collision(state, state->currentX, state->currentY, state->currentPiece,
                          state->currentRotation)) {
                state->gameOver = 1;
            }
        }

        clearScreen();
        drawStats(state);
        drawNextPiece(state);
        drawField(state);
        drawHints();
        drawFallingPiece(state);
        fflush(stdout);
        usleep(gameSpeed);
    }
}

int main(void) {
    struct termios orig_term, new_term;
    tcgetattr(STDIN_FILENO, &orig_term);
    new_term = orig_term;
    new_term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

    hideCursor();
    srand(time(NULL));

    GameState game;
    memset(&game, 0, sizeof(GameState));

    game.currentPiece = generateRandomPiece();
    game.currentRotation = 0;
    game.currentX = FIELD_WIDTH / 2 - MATRIX_SIZE / 2;
    game.currentY = 0;

    game.nextPiece = generateRandomPiece();
    game.nextRotation = 0;
    game.gameOver = 0;

    gameLoop(&game);

    moveCursor(FIELD_ROW, FIELD_COL);
    printf("Game Over!\n");
    moveCursor(BOTTOM_ROW, 0);

    showCursor();
    fflush(stdout);

    tcsetattr(STDIN_FILENO, TCSANOW, &orig_term);

    return 0;
}