CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -pedantic -O2

SRC := src/main.c src/program.c src/memory.c src/generator.c
OBJ := $(SRC:.c=.o)

memscope: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

clean:
	rm -f $(OBJ) memscope

.PHONY: clean
