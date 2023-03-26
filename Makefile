CC = mpicc
CFLAGS = -O3 -Wall -I./include
LDFLAGS = -fopenmp

all: main

main: main.o funcs.o
	$(CC) $(LDFLAGS) main.o funcs.o -o main

main.o: main.c funcs.h
	$(CC) $(CFLAGS) -c main.c

funcs.o: funcs.c funcs.h
	$(CC) $(CFLAGS) -c funcs.c

clean:
	rm -f *.o main
