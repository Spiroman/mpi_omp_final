build:
	mpicxx -fopenmp -c main.c 
	mpicxx -fopenmp -c funcs.c 
	mpicxx -fopenmp -o exec main.o funcs.o

clean:
	rm -f *.o ./exec

run:
	mpiexec -n 4 ./exec < small-input.txt