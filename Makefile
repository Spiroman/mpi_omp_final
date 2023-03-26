build:
	mpicxx -fopenmp -c main.c 
	mpicxx -fopenmp -c funcs.c 
	mpicxx -fopenmp -o match main.o funcs.o 

clean:
	rm -f *.o ./match

run:
	mpiexec -n 4 ./match < input.txt
