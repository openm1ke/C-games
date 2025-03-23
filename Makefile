CC=gcc
CFLAGS=-Wextra -Werror -Wall

FILES=snake pong game_of_life match_three stars tetris sokoban

all: $(FILES)

snake: clean snake.c
	$(CC) $(CFLAGS) snake.c -o snake && ./snake

pong: clean pong.c
	$(CC) $(CFLAGS) pong.c -o pong && ./pong

game_of_life: clean game_of_life.c
	$(CC) $(CFLAGS) game_of_life.c -o game_of_life && ./game_of_life

match_three: clean match_three.c
	$(CC) $(CFLAGS) match_three.c -o match_three && ./match_three

stars: clean stars.c
	$(CC) $(CFLAGS) stars.c -o stars && ./stars

tetris: clean tetris.c
	$(CC) $(CFLAGS) tetris.c -o tetris && ./tetris

sokoban: clean sokoban.c
	$(CC) $(CFLAGS) sokoban.c -o sokoban && ./sokoban

clean:
	rm -rf *.o $(FILES)

rebuild: clean all
