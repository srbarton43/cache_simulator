
.PHONY: all test clean

L = ./include
S = ./src

OBJS = $S/cache.o
CC = gcc
CFLAGS = -Wall -I$L

all: cache_simulation

cache_simulation: $S/cache_simulation.o
	$(CC) $(CFLAGS) -o $@ $< $(OBJS)

$S/cache_simulation.o: $S/cache_simulation.c $S/cache.o

$S/cache.o: $S/cache.c $L/cache.h
	$(CC) $(CFLAGS) -c $S/cache.c -o $@

test: cache_simulation
	bash -v tests/testing.sh

clean:
	rm -f cache_simulation src/*.o
