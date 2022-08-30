/*
 * P01D06 - C Ping-Pong
 * Programmer: Mikhail Shmyrev
 * 21-School username: quarkron
 * 06 Aug 2022
 */

// подключение заголовочного файла
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

// объявление констант
#define WIDTH 80      // ширина поля
#define HEIGHT 25     // выоста поля
#define MAX_SCORE 21  // максимальное количество очков для победы
#define SEPERATOR WIDTH / HEIGHT  // разделитель для вывода табло очков
#define R1X 1                     // начальная координата Х первой ракетки
#define R1Y HEIGHT / 2 - 1        // координата Y первой ракетки
#define R2X WIDTH                 // начальная координата X второй ракетки
#define R2Y R1Y                   // координата Y второй ракетки
// начальное положение мяча на поле (в центре)
#define BALL_X WIDTH / 2
#define BALL_Y HEIGHT / 2
// начальный вектор движения мяча (вверх-налево)
#define VECTOR_X -1
#define VECTOR_Y -1
// символы для построения игрового поля
#define WALL1 '-'  // верхний бортик
#define WALL2 '|'  // ракетка
#define BALL 'o'   // мяч
#define GRID '.'   // сетка
// начальное количество очков (0 у обоих игроков)
#define P1S 0
#define P2S 0

// декларирование функции для отрисовки игрового поля
void table(int r1x, int r1y, int r2x, int r2y, int ballx, int bally, int p1s, int p2s);

// основная функции запуска игры
int main() {
    // переменная для считывания ввода с клавиатуры
    char temp;
    char c = ' ';
    int n = 0;
    // присвоение локальным переменным исходных значений
    int r1x = R1X;
    int r1y = R1Y;
    int r2x = R2X;
    int r2y = R2Y;
    int p1s = P1S;
    int p2s = P2S;
    int ballx = BALL_X;
    int bally = BALL_Y;
    // задание начального вектора движения мяча
    int vector_x = VECTOR_X;
    int vector_y = VECTOR_Y;

    int time = 200000;

    // вызов функции прорисовки первый раз
    table(R1X, R1Y, R2X, R2Y, BALL_X, BALL_Y, P1S, P2S);

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
                printf("Pressed q (quit)\n");
                break;
            }
            if (c == 'a' || c == 'A')
                if (r1y > 1) r1y -= 1;
            if (c == 'z' || c == 'Z')
                if (r1y < HEIGHT - 2) r1y += 1;
            if (c == 'k' || c == 'K')
                if (r2y > 1) r2y -= 1;
            if (c == 'm' || c == 'M')
                if (r2y < HEIGHT - 2) r2y += 1;
        }
        system("clear");
        if (bally + vector_y < 0 || bally + vector_y > HEIGHT - 1) vector_y = -vector_y;
        if (ballx + vector_x == r1x &&
            (bally + vector_y == r1y || bally + vector_y == r1y - 1 || bally + vector_y == r1y + 1))
            vector_x = -vector_x;
        if (ballx + vector_x == r2x &&
            (bally + vector_y == r2y || bally + vector_y == r2y - 1 || bally + vector_y == r2y + 1))
            vector_x = -vector_x;

        ballx += vector_x;
        bally += vector_y;

        if (ballx <= r1x || ballx >= r2x) {
            if (ballx <= r1x) p2s += 1;
            if (ballx >= r2x) p1s += 1;
            vector_x = -vector_x;
            ballx = BALL_X;
            bally = BALL_Y;
        }
        table(r1x, r1y, r2x, r2y, ballx, bally, p1s, p2s);

        if (p1s == MAX_SCORE || p2s == MAX_SCORE) {
            printf("Press enter to start new game");
            if (scanf("%c", &temp) == 1) {
                p1s = P1S;
                p2s = P1S;
                r1x = R1X;
                r1y = R1Y;
                r2x = R2X;
                r2y = R2Y;
                ballx = BALL_X;
                bally = BALL_Y;
                table(r1x, r1y, r2x, r2y, ballx, bally, p1s, p2s);
            }
        }

        usleep(time);
    }

    // успешный выход из программы
    return 0;
}

/*
 * Основная функция прорисовки поля, ракеток и мяча
 * Входные параметры:
 * r1x, r1y - координаты левой ракетки
 * r2x, r2y - координаты правой ракетки
 * ballx, bally - координаты мяча
 * p1s, p2s - очки игрока1 и игрока2
 */

// объявление функции
void table(int racket1_x, int racket1_y, int racket2_x, int racket2_y, int ballx, int bally, int p1s,
           int p2s) {
    // построчный вывод в цикле
    for (int line = 0; line <= HEIGHT; line++) {
        // вывод верхнего и нижнего игрового бортика
        if (line == 0 || line == HEIGHT) {
            for (int column = 0; column <= WIDTH + 1; column++) {
                putchar(WALL1);
                if (column == WIDTH + 1) {
                    putchar('\n');
                }
            }
            if (line == HEIGHT) break;
        }

        // заполнение поля ракетками и шаром
        for (int column = 0; column <= WIDTH + 1; column++) {
            // вывод левой ракетки
            // ширина игровой ракетки 3 символа
            if (column == racket1_x &&
                (line == racket1_y || line == racket1_y - 1 || line == racket1_y + 1)) {
                putchar(WALL2);
                continue;
            }
            // вывод правой ракетки
            if (column == racket2_x &&
                (line == racket2_y || line == racket2_y - 1 || line == racket2_y + 1)) {
                putchar(WALL2);
                continue;
            }
            // игровой шар в виде символа "о"
            if (column == ballx && line == bally) {
                putchar(BALL);
                continue;
            }
            // количество очков игрока1
            if (line == 4 && column == WIDTH / 2 - SEPERATOR) {
                printf("%d", p1s);
                continue;
            }
            // количество очков игрока2
            if (line == 4 && column == WIDTH / 2 + SEPERATOR) {
                printf("%d", p2s);
                continue;
            }
            // по достижении максимального количество очков для победы
            // вывести имя игрока - победителя
            if (p1s == MAX_SCORE || p2s == MAX_SCORE) {
                if (line == 7 && column == WIDTH / 2 - SEPERATOR - 3) {
                    if (p1s == MAX_SCORE) {
                        printf("Player1 wins!\n");
                        break;
                    }
                    if (p2s == MAX_SCORE) {
                        printf("Player2 wins!\n");
                        break;
                    }
                }
            }

            // сетка на поле
            if (column == WIDTH / 2) {
                putchar(GRID);
                continue;
            }

            // заполнение поля пробелами
            putchar(' ');
            if (column == WIDTH + 1) {
                putchar('\n');
            }
        }
    }
}
