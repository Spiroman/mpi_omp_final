MPICC = mpicc

CFLAGS = -fopenmp

build:
	$(MPICC) $(CFLAGS) -c main.c
	$(MPICC) $(CFLAGS) -c funcs.c
	$(MPICC) main.o funcs.o -o exec -lgomp -lm

clean:
	rm -f *.o ./exec output.txt

run:
	mpiexec -n 4 ./exec 
	cat output.txt