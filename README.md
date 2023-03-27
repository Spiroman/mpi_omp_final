# Final project parrallel and distributed computing
The following project finds a given object in a given picture, where both those objects are nxn squares of integers.  
Finds is defined as such: for a given threshold (first float value in input file) for each overlapping position of object in picture it calculates the sum like so: (abs(picure[i]-object[i])/picture[i]). If the sum is smaller than the given threshold it counts that the object was found in the picture.  
Saves all results in an `output.txt` file
Examples of input files can be found in the inputs folder.

## Execution
There are 2 makefiles which can be used to run this. One for Linux and the other for MacOS.  
The default is for Linux.  
Please see the instructions below for execution in each system.  
There are also different input files, the default is the on in `inputs/input-medium.txt`

### Linux
This assumes you have `mpicc` installed and that is supports the `-fopenmp` flag.  
This runs without a problem on Afeka's Parallel Vlab machines.
1. `make`
2. `make run`
3. (optional) `make clean`

# MacOS
To run this on macos machine follow these steps from the project folder:
1. `brew install llvm libomp` - This installs the necessary compilers for `openmp` and `mpi`.
2. `cp makes/macos-make Makefile`
3. `make`
4. `make run`
5. (optional) `make clean`