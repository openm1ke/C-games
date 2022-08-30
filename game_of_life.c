#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#define WIDTH 80
#define HEIGHT 25
#define DEATH ' '
#define LIFE '*'
#define EMPTY '.'

void fill_matrix(int ***matrix);
void output_matrix(int ***matrix);
void free_matrix(int ***matrix);
void malloc_matrix(int ***matrix);
void swap_matrix(int ***prev_gen, int ***next_gen);
int count_closes(int x, int y, int ***matrix);
void new_generation(int ***prev_gen, int ***next_gen);
void check_matrix(int ***prev_gen, int ***next_gen);
void start_game();
static int generations = 0;

int main() {
    start_game();
    return 0;
}

void start_game() {
    int **new_gen, **prev_gen;
    char c = ' ';
    int n = 0;
    int time = 500000;

    malloc_matrix(&prev_gen);
    malloc_matrix(&new_gen);

    fill_matrix(&prev_gen);

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
            if (c == 'q') {
                printf("Pressed q (quit), last generation is %d\n", generations);
                break;
            }
        }
        system("clear");
        output_matrix(&prev_gen);
        new_generation(&prev_gen, &new_gen);

        check_matrix(&prev_gen, &new_gen);
        generations++;
        swap_matrix(&prev_gen, &new_gen);

        usleep(time);
    }

    free_matrix(&new_gen);
    free_matrix(&prev_gen);
}

void swap_matrix(int ***prev_gen, int ***new_gen) {
    int **temp = (*prev_gen);
    (*prev_gen) = (*new_gen);
    (*new_gen) = temp;
    temp = NULL;
}

void malloc_matrix(int ***matrix) {
    *matrix = calloc(HEIGHT, sizeof(int *));
    for (int i = 0; i < HEIGHT; i++) {
        (*matrix)[i] = calloc(WIDTH, sizeof(int));
    }
}

void check_matrix(int ***prev_gen, int ***next_gen) {
    int flag = 1;
    int **future_gen;
    malloc_matrix(&future_gen);

    new_generation(next_gen, &future_gen);

    int count = 0;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (future_gen[i][j] == 1) count++;
        }
    }

    if (!count) {
        printf("There is no future (gens = %d)\n", generations);
        free_matrix(&future_gen);
        free_matrix(prev_gen);
        free_matrix(next_gen);
        future_gen = NULL;
        exit(0);
    }

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if ((*prev_gen)[i][j] != future_gen[i][j]) {
                flag = 0;
            }
        }
    }
    if (flag) {
        printf("This is the end (gens = %d)\n", generations);
        free_matrix(&future_gen);
        free_matrix(prev_gen);
        free_matrix(next_gen);
        exit(0);
    }
    free_matrix(&future_gen);
}

void new_generation(int ***prev_gen, int ***next_gen) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            int current = (*prev_gen)[i][j];
            int neighbors = count_closes(i, j, prev_gen);
            if (current == 0 && neighbors == 3) {
                (*next_gen)[i][j] = 1;
            } else if (current == 1 && (neighbors < 2 || neighbors > 3)) {
                (*next_gen)[i][j] = 0;
            } else {
                (*next_gen)[i][j] = current;
            }
        }
    }
}

int count_closes(int x, int y, int ***matrix) {
    int count = 0;
    int col, row;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            row = (x + i + HEIGHT) % HEIGHT;
            col = (y + j + WIDTH) % WIDTH;
            count += (*matrix)[row][col];
        }
    }
    count -= (*matrix)[x][y];
    return count;
}

void fill_matrix(int ***matrix) {
    /*
    int c;
    int i = 0, j = 0;
    while ((c = getchar())) {
        if (c == EOF) break;
        if (c == 10) continue;
        if (c == EMPTY) {
            (*matrix)[i][j] = 0;
        } else {
            (*matrix)[i][j] = 1;
        }
        j++;
        if (j == WIDTH) {
            i++;
            j = 0;
        }
        if (i + 1 == HEIGHT) break;
    }
    */
    //  заполнение случайными значениями (для тестов)
    srand(time(0));
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            (*matrix)[i][j] = rand() % 100 + 1 > 85 ? 1 : 0;
        }
    }
}

void output_matrix(int ***matrix) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if ((*matrix)[i][j] == 0) {
                putchar(DEATH);
            } else {
                putchar(LIFE);
            }
        }
        putchar('\n');
    }
}

void free_matrix(int ***matrix) {
    for (int i = 0; i < HEIGHT; i++) {
        free((*matrix)[i]);
    }
    free((*matrix));
}
