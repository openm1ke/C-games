CC=gcc
CFLAGS=-Wextra -Werror -Wall
FILES=snake pong game_of_life match_three stars

all: $(FILES)

snake: snake.c
	$(CC) $(CFLAGS) snake.c -o snake

pong: pong.c
	$(CC) $(CFLAGS) pong.c -o pong

game_of_life: game_of_life.c
	$(CC) $(CFLAGS) game_of_life.c -o game_of_life

match_three: match_three.c
	$(CC) $(CFLAGS) match_three.c -o match_three

stars: stars.c
	$(CC) $(CFLAGS) stars.c -o stars

clean:
	rm -rf *.o $(FILES)

rebuild: clean all
