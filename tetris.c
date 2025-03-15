#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NUM_PIECES 7
#define NUM_ROTATIONS 4
#define MATRIX_SIZE 4

#define FIELD_WIDTH 10      // Ширина игрового поля в клетках
#define FIELD_HEIGHT 20     // Высота игрового поля в клетках
#define CELL_WIDTH 2        // Каждая клетка выводится как 2 символа ("[]" или "  ")
#define BOTTOM_ROW 24

// Определяем позиции зон на экране (номер строки и столбца)
#define STATS_ROW 8
#define STATS_COL 1

#define NEXT_PIECE_ROW 13
#define NEXT_PIECE_COL 1

#define FIELD_ROW 1
#define FIELD_COL 15

#define HINTS_ROW 8
#define HINTS_COL 42

// Определяем четырехмерный массив для всех фигур.
char tetromino[NUM_PIECES][NUM_ROTATIONS][MATRIX_SIZE][MATRIX_SIZE] = {
    // Фигура I
    {
        {   // 0°: .... XXXX .... ....
            {'.', '.', '.', '.'},
            {'X', 'X', 'X', 'X'},
            {'.', '.', '.', '.'},
            {'.', '.', '.', '.'}
        },
        {   // 90°: ..X. ..X. ..X. ..X.
            {'.', '.', 'X', '.'},
            {'.', '.', 'X', '.'},
            {'.', '.', 'X', '.'},
            {'.', '.', 'X', '.'}
        },
        {   // 180°: .... .... XXXX ....
            {'.', '.', '.', '.'},
            {'.', '.', '.', '.'},
            {'X', 'X', 'X', 'X'},
            {'.', '.', '.', '.'}
        },
        {   // 270°: .X.. .X.. .X.. .X..
            {'.', 'X', '.', '.'},
            {'.', 'X', '.', '.'},
            {'.', 'X', '.', '.'},
            {'.', 'X', '.', '.'}
        }
    },
    // Фигура T
    {
        {   // 0°:
            {'.', '.', '.', '.'},
            {'.', 'X', 'X', 'X'},
            {'.', '.', 'X', '.'},
            {'.', '.', '.', '.'}
        },
        {   // 90°:
            {'.', '.', '.', '.'},
            {'.', '.', 'X', '.'},
            {'.', 'X', 'X', '.'},
            {'.', '.', 'X', '.'}
        },
        {   // 180°:
            {'.', '.', '.', '.'},
            {'.', 'X', '.', '.'},
            {'X', 'X', 'X', '.'},
            {'.', '.', '.', '.'}
        },
        {   // 270°:
            {'.', 'X', '.', '.'},
            {'.', 'X', 'X', '.'},
            {'.', 'X', '.', '.'},
            {'.', '.', '.', '.'}
        }
    },
    // Фигура O (квадрат)
    {
        {   // 0°:
            {'.', '.', '.', '.'},
            {'.', 'X', 'X', '.'},
            {'.', 'X', 'X', '.'},
            {'.', '.', '.', '.'}
        },
        // У квадрата все повороты одинаковы
        {
            {'.', '.', '.', '.'},
            {'.', 'X', 'X', '.'},
            {'.', 'X', 'X', '.'},
            {'.', '.', '.', '.'}
        },
        {
            {'.', '.', '.', '.'},
            {'.', 'X', 'X', '.'},
            {'.', 'X', 'X', '.'},
            {'.', '.', '.', '.'}
        },
        {
            {'.', '.', '.', '.'},
            {'.', 'X', 'X', '.'},
            {'.', 'X', 'X', '.'},
            {'.', '.', '.', '.'}
        }
    },
    // Фигура L
    {
        {   // 0°:
            {'.', 'X', 'X', '.'},
            {'.', '.', 'X', '.'},
            {'.', '.', 'X', '.'},
            {'.', '.', '.', '.'}
        },
        {   // 90°:
            {'.', '.', '.', '.'},
            {'.', '.', 'X', '.'},
            {'X', 'X', 'X', '.'},
            {'.', '.', '.', '.'}
        },
        {   // 180°:
            {'.', 'X', '.', '.'},
            {'.', 'X', '.', '.'},
            {'.', 'X', 'X', '.'},
            {'.', '.', '.', '.'}
        },
        {   // 270°:
            {'.', '.', '.', '.'},
            {'.', 'X', 'X', 'X'},
            {'.', 'X', '.', '.'},
            {'.', '.', '.', '.'}
        }
    },
    // Фигура J
    {
        {   // 0°:
            {'.', 'X', 'X', '.'},
            {'.', 'X', '.', '.'},
            {'.', 'X', '.', '.'},
            {'.', '.', '.', '.'}
        },
        {   // 90°:
            {'.', '.', '.', '.'},
            {'X', 'X', 'X', '.'},
            {'.', '.', 'X', '.'},
            {'.', '.', '.', '.'}
        },
        {   // 180°:
            {'.', '.', 'X', '.'},
            {'.', '.', 'X', '.'},
            {'.', 'X', 'X', '.'},
            {'.', '.', '.', '.'}
        },
        {   // 270°:
            {'.', '.', '.', '.'},
            {'.', 'X', '.', '.'},
            {'.', 'X', 'X', 'X'},
            {'.', '.', '.', '.'}
        }
    },
    // Фигура S
    {
        {   // 0°:
            {'.', '.', '.', '.'},
            {'.', 'X', 'X', '.'},
            {'X', 'X', '.', '.'},
            {'.', '.', '.', '.'}
        },
        {   // 90°:
            {'.', 'X', '.', '.'},
            {'.', 'X', 'X', '.'},
            {'.', '.', 'X', '.'},
            {'.', '.', '.', '.'}
        },
        {   // 180°:
            {'.', '.', '.', '.'},
            {'.', '.', 'X', 'X'},
            {'.', 'X', 'X', '.'},
            {'.', '.', '.', '.'}
        },
        {   // 270°:
            {'.', '.', '.', '.'},
            {'.', 'X', '.', '.'},
            {'.', 'X', 'X', '.'},
            {'.', '.', 'X', '.'}
        }
    },
    // Фигура Z
    {
        {   // 0°:
            {'.', '.', '.', '.'},
            {'X', 'X', '.', '.'},
            {'.', 'X', 'X', '.'},
            {'.', '.', '.', '.'}
        },
        {   // 90°:
            {'.', '.', 'X', '.'},
            {'.', 'X', 'X', '.'},
            {'.', 'X', '.', '.'},
            {'.', '.', '.', '.'}
        },
        {   // 180°:
            {'.', '.', '.', '.'},
            {'.', 'X', 'X', '.'},
            {'.', '.', 'X', 'X'},
            {'.', '.', '.', '.'}
        },
        {   // 270°:
            {'.', '.', '.', '.'},
            {'.', '.', 'X', '.'},
            {'.', 'X', 'X', '.'},
            {'.', 'X', '.', '.'}
        }
    }
};

// Игровое поле: 0 - пустая клетка, 1 - заполненная
int field[FIELD_HEIGHT][FIELD_WIDTH];

// Глобальные переменные для падающей фигуры
int falling_piece = 1;      // Например, T-тетромино
int falling_rotation = 0;     // Исходное положение
int falling_x = 3;          // Начальное положение по горизонтали (в клетках)
int falling_y = 0;          // Начальное положение по вертикали

// Функция для очистки экрана
void clearScreen() {
    printf("\033[H\033[J");
}

// Функция для перемещения курсора в заданную позицию
void moveCursor(int row, int col) {
    printf("\033[%d;%dH", row, col);
}

// Функция вывода статистики (сгоревшие линии, уровень, очки)
void drawStats() {
    moveCursor(STATS_ROW, STATS_COL);
    printf("Lines: %d", 5);
    moveCursor(STATS_ROW + 1, STATS_COL);
    printf("Level: %d", 2);
    moveCursor(STATS_ROW + 2, STATS_COL);
    printf("Score: %d", 1234);
}

// Функция вывода следующей фигуры из массива tetromino
void drawNextPiece() {
    int next_piece = 2;     // Например, следующая фигура — квадрат (O)
    int next_rotation = 0;  // У квадрата все повороты одинаковы
    moveCursor(NEXT_PIECE_ROW, NEXT_PIECE_COL);
    printf("Next Piece:");
    for (int i = 0; i < MATRIX_SIZE; i++) {
        moveCursor(NEXT_PIECE_ROW + i + 1, NEXT_PIECE_COL);
        for (int j = 0; j < MATRIX_SIZE; j++) {
            char c = tetromino[next_piece][next_rotation][i][j];
            if (c == 'X')
                printf("[]");
            else
                printf("  ");
        }
    }
}

// Функция вывода игрового поля с рамкой
void drawField() {
    // Верхняя граница поля
    moveCursor(FIELD_ROW, FIELD_COL);
    printf("<!");
    for (int i = 0; i < FIELD_WIDTH * CELL_WIDTH; i++) {
        printf(" ");
    }
    printf("!>");
    
    // Вывод содержимого игрового поля
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        moveCursor(FIELD_ROW + 1 + y, FIELD_COL);
        printf("<!");  // Левая граница
        for (int x = 0; x < FIELD_WIDTH; x++) {
            if (field[y][x] == 0)
                printf("  ");
            else
                printf("[]");
        }
        printf("!>"); // Правая граница
    }
    
    // Нижняя граница поля (строка из "=")
    int bottomRow = FIELD_ROW + 1 + FIELD_HEIGHT;
    moveCursor(bottomRow, FIELD_COL);
    printf("<!");
    for (int i = 0; i < FIELD_WIDTH * CELL_WIDTH; i++) {
        printf("=");
    }
    printf("!>");
    
    // Декоративная строка под нижней границей (шаблон /\/\)
    moveCursor(bottomRow + 1, FIELD_COL + 2);
    for (int i = 0; i < FIELD_WIDTH; i++) {
        printf("\\/");
    }
}

// Функция вывода подсказок по управлению игрой
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

// Функция, рисующая падающую фигуру поверх поля
// Выводится из массива tetromino с учётом её позиции (falling_x, falling_y)
void drawFallingPiece() {
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            if (tetromino[falling_piece][falling_rotation][i][j] == 'X') {
                // Рассчитываем позицию на экране:
                // Содержимое игрового поля выводится начиная с FIELD_ROW+1 и FIELD_COL+2
                int screen_row = FIELD_ROW + 1 + falling_y + i;
                int screen_col = FIELD_COL + 2 + (falling_x + j) * CELL_WIDTH;
                moveCursor(screen_row, screen_col);
                printf("[]");
            }
        }
    }
}

// Тестовая функция, которая опускает падающую фигуру до дна
void dropTest() {
    // Будем двигать фигуру вниз до тех пор, пока её нижняя часть не достигнет нижней границы поля
    while (falling_y < FIELD_HEIGHT - MATRIX_SIZE + 1) {
        clearScreen();
        drawStats();
        drawNextPiece();
        drawField();
        drawHints();
        drawFallingPiece();
        // Обновляем экран
        fflush(stdout);
        // Ждём некоторое время (начальная скорость)
        usleep(300000); // 300 мс, можно регулировать
        // Опускаем фигуру на одну клетку
        falling_y++;
    }
}

void setCursorDown() {
    moveCursor(BOTTOM_ROW, 0);
    printf("\033[?25h");
}

void hideCursor() {
    printf("\033[?25l");
}

void showCursor() {
    printf("\033[?25h");
}

int main(void) {
    hideCursor();
    // Инициализируем игровое поле тестовыми данными:
    memset(field, 0, sizeof(field));
    for (int x = 0; x < FIELD_WIDTH; x++) {
        field[FIELD_HEIGHT - 1][x] = (x % 2 == 0) ? 1 : 0;
    }
    field[10][3] = 1;
    field[10][4] = 1;
    field[11][4] = 1;
    
    // Сбрасываем позицию падающей фигуры
    falling_piece = 1;      // T-тетромино
    falling_rotation = 0;
    falling_x = 3;
    falling_y = 0;
    
    // Запускаем тестовую функцию падения
    dropTest();
    
    // После завершения теста устанавливаем курсор вниз и ждём перед выходом
    setCursorDown();
    showCursor();
    fflush(stdout);
    sleep(3);
    
    return 0;
}