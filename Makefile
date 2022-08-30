CC=gcc
CFLAGS=-Wextra -Werror -Wall

all: snake pong game_of_life

snake: snake.c
	$(CC) $(CFLAGS) snake.c -o snake

pong: pong.c
	$(CC) $(CFLAGS) pong.c -o pong

game_of_life: game_of_life.c
	$(CC) $(CFLAGS) game_of_life.c -o game_of_life

clean:
	rm -rf *.o snake pong game_of_life

rebuild: clean all
