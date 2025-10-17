CC = gcc
CFLAGS = -Wall -I./ -I./lib/cJSON
# add -Wextra to flag
TEST = ./test/*.c
LIB_SRC = lib/cJSON/cJSON.c
SRC := $(wildcard ./*.c)
SRC := $(filter-out ./main.c, $(SRC))

# Build the main program
game: 
	$(CC) $(SRC) main.c $(LIB_SRC) $(CFLAGS) $^ -o game.out

# Build the tests and test
test: 
	$(CC) $(SRC) $(TEST) ./test/main.c $(LIB_SRC) $(CFLAGS) $^ -o test.out && ./test.out

# Build 
test-game: game 
	./game.out

clean:
	rm -f game test

