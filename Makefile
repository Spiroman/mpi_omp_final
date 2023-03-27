CC = /usr/local/opt/llvm/bin/clang
MPICC = mpicc

CFLAGS = -fopenmp
MPI_COMPILE_FLAGS = $(shell $(MPICC) --showme:compile)
OPENMP_MACOS_INCLUDE = -I/usr/local/opt/libomp/include
OPENMP_MACOS_LIB = -L/usr/local/opt/libomp/lib
OPENMP_MACOS_FLAGS = -Xpreprocessor -fopenmp

build:
	$(CC) $(CFLAGS) $(MPI_COMPILE_FLAGS) $(OPENMP_MACOS_INCLUDE) $(OPENMP_MACOS_FLAGS) -c main.c
	$(CC) $(CFLAGS) $(MPI_COMPILE_FLAGS) $(OPENMP_MACOS_INCLUDE) $(OPENMP_MACOS_FLAGS) -c funcs.c
	$(MPICC) main.o funcs.o -o exec $(OPENMP_MACOS_LIB) -lomp

clean:
	rm -f *.o ./exec

run:
	mpiexec -n 4 ./exec < input-medium.txt