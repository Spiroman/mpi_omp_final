CC = mpicc
CFLAGS = -Wall -O3 -fopenmp
LIBS = -lm
TARGET = main
OBJECTS = main.o funcs.o
HEADERS = funcs.h data.h

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

main.o: main.c $(HEADERS)
	$(CC) $(CFLAGS) -c main.c

funcs.o: funcs.c funcs.h data.h
	$(CC) $(CFLAGS) -c funcs.c

clean:
	rm -f $(TARGET) $(OBJECTS)
