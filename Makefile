
.PHONY: all test clean

L = ./include
S = ./src

OBJS = $S/cache.o
CC = gcc
CFLAGS = -Wall -pedantic -I$L

all: cache_simulation

cache_simulation: $S/cache_simulation.o
	$(CC) $(CFLAGS) -o $@ $< $(OBJS)

src/cache_simulation.o: $S/cache_simulation.c $S/cache.o

src/cache.o: $L/cache.h
	$(CC) $(CFLAGS) -c $S/cache.c -o $@

test: cache_simulation
	bash -v testing.sh

clean:
	rm -f cache_simulation src/*.o
