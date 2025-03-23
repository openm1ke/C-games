#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define MAX_ROWS 20
#define MAX_COLS 40

#define FIELD_ROW 3
#define FIELD_COL 3
#define GAP 4

int level_rows, level_cols;
char base[MAX_ROWS][MAX_COLS + 1];
char objects[MAX_ROWS][MAX_COLS];
int player_r, player_c;

int current_level_index = 0;
int total_steps = 0;

typedef struct State {
    char objects[MAX_ROWS][MAX_COLS];
    int player_r;
    int player_c;
    int step_count;
    struct State *next;
} State;
State *undo_stack = NULL;

void push_state() {
    State *s = malloc(sizeof(State));
    for (int r = 0; r < level_rows; r++)
        for (int c = 0; c < level_cols; c++) s->objects[r][c] = objects[r][c];
    s->player_r = player_r;
    s->player_c = player_c;
    s->step_count = total_steps;
    s->next = undo_stack;
    undo_stack = s;
}

int pop_state() {
    if (undo_stack == NULL) return 0;
    State *s = undo_stack;
    for (int r = 0; r < level_rows; r++)
        for (int c = 0; c < level_cols; c++) objects[r][c] = s->objects[r][c];
    player_r = s->player_r;
    player_c = s->player_c;
    total_steps = s->step_count;
    undo_stack = s->next;
    free(s);
    return 1;
}

void clear_undo_stack() {
    while (undo_stack) {
        State *tmp = undo_stack;
        undo_stack = undo_stack->next;
        free(tmp);
    }
}

const char *hints[] = {"Controls:", "A - Left", "D - Right",   "W - Up",
                       "S - Down",  "Z - Undo", "R - Restart", "Q - Quit"};
int hint_count = sizeof(hints) / sizeof(hints[0]);

typedef struct {
    int rows;
    int cols;
    const char **map;
} Level;

const char *level1_map[] = {"#######", "#.   .#", "#  B  #", "# B@B #", "#  B  #", "#.   .#", "#######"};
const char *level2_map[] = {"########", "#.    .#", "#@BBBB #", "#.    .#", "########"};

Level levels[] = {{7, 7, level1_map}, {5, 8, level2_map}};
int total_levels = sizeof(levels) / sizeof(Level);

void clearScreen() { printf("\033[2J\033[H"); }

void moveCursor(int row, int col) { printf("\033[%d;%dH", row, col); }

void hideCursor() { printf("\033[?25l"); }
void showCursor() { printf("\033[?25h"); }

void load_level(int index) {
    clear_undo_stack();
    Level lvl = levels[index];
    level_rows = lvl.rows;
    level_cols = lvl.cols;
    for (int r = 0; r < level_rows; r++) {
        for (int c = 0; c < level_cols; c++) {
            char ch = lvl.map[r][c];
            if (ch == '#') {
                base[r][c] = '#';
                objects[r][c] = ' ';
            } else if (ch == '.') {
                base[r][c] = '.';
                objects[r][c] = ' ';
            } else if (ch == ' ') {
                base[r][c] = ' ';
                objects[r][c] = ' ';
            } else if (ch == '@') {
                base[r][c] = ' ';
                objects[r][c] = '@';
                player_r = r;
                player_c = c;
            } else if (ch == 'B') {
                base[r][c] = ' ';
                objects[r][c] = 'B';
            } else {
                base[r][c] = ' ';
                objects[r][c] = ' ';
            }
        }
        base[r][level_cols] = '\0';
    }
    total_steps = 0;
    push_state();
}

void drawStatusLine() {
    moveCursor(1, FIELD_COL);
    printf("Level: %d/%d   Steps: %d", current_level_index + 1, total_levels, total_steps);
}

void drawField() {
    for (int r = 0; r < level_rows; r++) {
        moveCursor(FIELD_ROW + r, FIELD_COL);
        for (int c = 0; c < level_cols; c++) {
            if (objects[r][c] != ' ')
                printf("%c", objects[r][c]);
            else
                printf("%c", base[r][c]);
        }
    }
}

void drawHints() {
    int hint_col = FIELD_COL + level_cols + GAP;
    for (int i = 0; i < hint_count; i++) {
        moveCursor(FIELD_ROW + i, hint_col);
        printf("%s", hints[i]);
    }
}

void showLevelComplete() {
    moveCursor(FIELD_ROW + level_rows + 1, FIELD_COL);
    printf("Level %d completed! Press any key to continue...", current_level_index + 1);
}

int is_within_bounds(int r, int c) { return (r >= 0 && r < level_rows && c >= 0 && c < level_cols); }

int move_player(char input) {
    int dr = 0, dc = 0;
    if (input == 'w' || input == 'W')
        dr = -1;
    else if (input == 's' || input == 'S')
        dr = 1;
    else if (input == 'a' || input == 'A')
        dc = -1;
    else if (input == 'd' || input == 'D')
        dc = 1;
    else
        return 0;

    int new_r = player_r + dr;
    int new_c = player_c + dc;
    if (!is_within_bounds(new_r, new_c)) return 0;
    if (base[new_r][new_c] == '#') return 0;

    if (objects[new_r][new_c] == ' ') {
        push_state();
        objects[player_r][player_c] = ' ';
        objects[new_r][new_c] = '@';
        player_r = new_r;
        player_c = new_c;
        return 1;
    } else if (objects[new_r][new_c] == 'B') {
        int box_new_r = new_r + dr;
        int box_new_c = new_c + dc;
        if (!is_within_bounds(box_new_r, box_new_c)) return 0;
        if (base[box_new_r][box_new_c] == '#') return 0;
        if (objects[box_new_r][box_new_c] != ' ') return 0;
        push_state();
        objects[box_new_r][box_new_c] = 'B';
        objects[new_r][new_c] = '@';
        objects[player_r][player_c] = ' ';
        player_r = new_r;
        player_c = new_c;
        return 1;
    }
    return 0;
}

int check_level_complete() {
    for (int r = 0; r < level_rows; r++) {
        for (int c = 0; c < level_cols; c++) {
            if (base[r][c] == '.') {
                if (objects[r][c] != 'B') return 0;
            }
        }
    }
    return 1;
}

void reset_level() { load_level(current_level_index); }

int main(void) {
    struct termios oldt, newt;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    hideCursor();
    load_level(current_level_index);

    char input;
    while (1) {
        clearScreen();
        drawStatusLine();
        drawField();
        // drawHints();
        fflush(stdout);

        input = getchar();
        if (input == 'q' || input == 'Q')
            break;
        else if (input == 'r' || input == 'R') {
            reset_level();
            continue;
        } else if (input == 'z' || input == 'Z') {
            pop_state();
            continue;
        }

        if (move_player(input)) total_steps++;

        if (check_level_complete()) {
            clearScreen();
            drawField();
            showLevelComplete();
            fflush(stdout);
            getchar();
            current_level_index++;
            if (current_level_index >= total_levels) {
                clearScreen();
                moveCursor(FIELD_ROW, FIELD_COL);
                printf("Congratulations! You have completed all levels!\n\n\n");
                break;
            }
            load_level(current_level_index);
        }
    }

    clear_undo_stack();
    showCursor();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return 0;
}
