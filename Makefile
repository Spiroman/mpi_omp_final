CC = mpicc
CFLAGS = -Wall -Wextra -g -O2
LDFLAGS = -lm

OBJECTS = main.o

all: program

program: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o program

main.o: main.c data.h
	$(CC) $(CFLAGS) -c main.c -o main.o

clean:
	rm -f $(OBJECTS) program
