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

// Функция вывода следующей фигуры (пример: T-тетромино)
void drawNextPiece() {
    const char *next_piece[4] = {
        "    ",
        " XXX",
        "  X ",
        "    "
    };
    moveCursor(NEXT_PIECE_ROW, NEXT_PIECE_COL);
    printf("Next Piece:");
    for (int i = 0; i < 4; i++) {
        moveCursor(NEXT_PIECE_ROW + i + 1, NEXT_PIECE_COL);
        for (int j = 0; j < 4; j++) {
            char c = next_piece[i][j];
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

void setCursorDown() {
    moveCursor(BOTTOM_ROW, 0);
}

int main(void) {
    // Инициализируем игровое поле тестовыми данными:
    // Заполняем поле нулями, затем для примера в нижней строке и в середине ставим заполненные клетки.
    memset(field, 0, sizeof(field));
    for (int x = 0; x < FIELD_WIDTH; x++) {
        // Пример: заполняем каждую вторую клетку в нижней строке
        field[FIELD_HEIGHT - 1][x] = (x % 2 == 0) ? 1 : 0;
    }
    // Несколько заполненных клеток в середине
    field[10][3] = 1;
    field[10][4] = 1;
    field[11][4] = 1;
    
    // Очищаем экран и выводим все зоны
    clearScreen();
    drawStats();
    drawNextPiece();
    drawField();
    drawHints();
    setCursorDown();
    fflush(stdout);
    // Ждём 5 секунд, чтобы можно было увидеть результат
    //sleep(15);
    
    return 0;
}
