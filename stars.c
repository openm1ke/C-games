#define _XOPEN_SOURCE 500
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define NUM_STARS 100
#define NUM_METEORITES 5
#define SPACESHIP_HEIGHT 5
#define SPACESHIP_WIDTH 10
#define MAX_PROJECTILES 10

int main() {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    printf("\033[?25l");
    fflush(stdout);

    srand(time(NULL));
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    int rows = ws.ws_row;
    int cols = ws.ws_col;
    int star_x[NUM_STARS], star_y[NUM_STARS];
    for (int i = 0; i < NUM_STARS; i++) {
        star_x[i] = rand() % cols;
        star_y[i] = rand() % rows;
    }
    int meteor_x[NUM_METEORITES], meteor_y[NUM_METEORITES];
    int meteor_alive[NUM_METEORITES], meteor_explosion_timer[NUM_METEORITES];
    for (int i = 0; i < NUM_METEORITES; i++) {
        meteor_x[i] = rand() % cols;
        meteor_y[i] = rand() % rows;
        meteor_alive[i] = 1;
        meteor_explosion_timer[i] = 0;
    }
    const char *spaceship[SPACESHIP_HEIGHT] = {"    /\\    ", "   /  \\   ", "  |----|  ", " /| [] |\\ ",
                                               "/_|____|_\\"};
    int ship_x = (cols - SPACESHIP_WIDTH) / 2;
    int ship_y = (rows - SPACESHIP_HEIGHT) / 2;
    int proj_x[MAX_PROJECTILES], proj_y[MAX_PROJECTILES], proj_active[MAX_PROJECTILES];
    for (int i = 0; i < MAX_PROJECTILES; i++) {
        proj_active[i] = 0;
    }

    while (1) {
        printf("\033[H\033[J");
        for (int i = 0; i < NUM_STARS; i++) {
            if (star_y[i] >= 0 && star_y[i] < rows && star_x[i] >= 0 && star_x[i] < cols) {
                printf("\033[%d;%dH.", star_y[i] + 1, star_x[i] + 1);
            }
        }
        for (int i = 0; i < NUM_METEORITES; i++) {
            if (meteor_alive[i]) {
                if (meteor_y[i] >= 0 && meteor_y[i] < rows && meteor_x[i] >= 0 && meteor_x[i] < cols) {
                    printf("\033[%d;%dH@", meteor_y[i] + 1, meteor_x[i] + 1);
                }
            } else {
                if (meteor_explosion_timer[i] > 0) {
                    if (meteor_y[i] >= 0 && meteor_y[i] < rows && meteor_x[i] >= 0 && meteor_x[i] < cols) {
                        printf("\033[%d;%dHX", meteor_y[i] + 1, meteor_x[i] + 1);
                    }
                }
            }
        }
        for (int i = 0; i < SPACESHIP_HEIGHT; i++) {
            printf("\033[%d;%dH%s", ship_y + i + 1, ship_x + 1, spaceship[i]);
        }
        for (int i = 0; i < MAX_PROJECTILES; i++) {
            if (proj_active[i]) {
                if (proj_y[i] >= 0 && proj_y[i] < rows && proj_x[i] >= 0 && proj_x[i] < cols) {
                    printf("\033[%d;%dH|", proj_y[i] + 1, proj_x[i] + 1);
                }
            }
        }

        fflush(stdout);
        int c;
        while ((c = getchar()) != EOF) {
            if (c == 'q' || c == 'Q') {
                goto cleanup;
            } else if (c == 'a' || c == 'A') {
                ship_x--;
            } else if (c == 'd' || c == 'D') {
                ship_x++;
            } else if (c == 'w' || c == 'W') {
                ship_y--;
            } else if (c == 's' || c == 'S') {
                ship_y++;
            } else if (c == ' ') {  // выстрел по пробелу
                for (int j = 0; j < MAX_PROJECTILES; j++) {
                    if (!proj_active[j]) {
                        proj_x[j] = ship_x + SPACESHIP_WIDTH / 2;
                        proj_y[j] = ship_y;
                        proj_active[j] = 1;
                        break;
                    }
                }
            }
        }
        for (int i = 0; i < NUM_STARS; i++) {
            star_y[i]++;
            if (star_y[i] >= rows) {
                star_y[i] = 0;
                star_x[i] = rand() % cols;
            }
        }
        for (int i = 0; i < NUM_METEORITES; i++) {
            if (meteor_alive[i]) {
                meteor_y[i]++;
                if (meteor_y[i] >= rows) {
                    meteor_y[i] = 0;
                    meteor_x[i] = rand() % cols;
                }
            } else {
                if (meteor_explosion_timer[i] > 0) {
                    meteor_explosion_timer[i]--;
                } else {
                    meteor_alive[i] = 1;
                    meteor_y[i] = 0;
                    meteor_x[i] = rand() % cols;
                }
            }
        }
        for (int i = 0; i < MAX_PROJECTILES; i++) {
            if (proj_active[i]) {
                proj_y[i]--;
                if (proj_y[i] < 0) proj_active[i] = 0;
            }
        }
        for (int i = 0; i < MAX_PROJECTILES; i++) {
            if (proj_active[i]) {
                for (int j = 0; j < NUM_METEORITES; j++) {
                    if (meteor_alive[j]) {
                        if (proj_x[i] == meteor_x[j] && proj_y[i] == meteor_y[j]) {
                            proj_active[i] = 0;
                            meteor_alive[j] = 0;
                            meteor_explosion_timer[j] = 5;  // длительность взрыва (5 кадров)
                        }
                    }
                }
            }
        }
        for (int i = 0; i < NUM_METEORITES; i++) {
            if (meteor_alive[i]) {
                if (meteor_x[i] >= ship_x && meteor_x[i] < ship_x + SPACESHIP_WIDTH &&
                    meteor_y[i] >= ship_y && meteor_y[i] < ship_y + SPACESHIP_HEIGHT) {
                    goto game_over;
                }
            }
        }
        if (ship_x < 0) ship_x = 0;
        if (ship_x > cols - SPACESHIP_WIDTH) ship_x = cols - SPACESHIP_WIDTH;
        if (ship_y < 0) ship_y = 0;
        if (ship_y > rows - SPACESHIP_HEIGHT) ship_y = rows - SPACESHIP_HEIGHT;

        usleep(100000);
    }

game_over : {
    const char *msg = "GAME OVER";
    int msg_len = strlen(msg);
    int msg_row = rows / 2 + 1;
    int msg_col = (cols - msg_len) / 2;
    printf("\033[%d;%dH%s", msg_row, msg_col + 1, msg);
    fflush(stdout);
}
    while (getchar() == EOF) {
        usleep(100000);
    }

cleanup:
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    printf("\033[?25h");
    printf("\033[H\033[J");
    return 0;
}
