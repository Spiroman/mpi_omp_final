#include <omp.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define MAX_NUMBER 100
#define SIZE 50000

void generateNumbers(int* arr, int size);
int isPrime(int number);
int countPrimes(int* arr, int size);
int countPrimesSeq(int* arr, int size);

int main(int argc, char* argv[])
{
	int arr[SIZE];

	generateNumbers(arr, SIZE);

    double t0 = omp_get_wtime();
	
	int count = countPrimes(arr, SIZE);

	double t1 = omp_get_wtime();


	printf("Number of primes: %d\n", count);
	printf("Time(parallel version): %lf seconds\n", t1 - t0);

    t0 = omp_get_wtime();
	
	count = countPrimesSeq(arr, SIZE);

	t1 = omp_get_wtime();


	printf("Number of primes: %d\n", count);
	printf("Time(sequential version): %lf seconds\n", t1 - t0);

	return 0;
}

int isPrime(int number)
{
	for (int i = 2; i < number ; i++)
	{
		if (number % i == 0) 
			return 0;
	}
	return 1;
}

int countPrimes(int* arr, int size)
{
	int maxThreads = omp_get_max_threads();
	int* counters = (int*)calloc(maxThreads, sizeof(int));

	#pragma omp parallel default(none) shared(counters, arr, size)
	{
		int threadId = omp_get_thread_num();
		int numOfThreads = omp_get_num_threads();

        for(int i = threadId; i < size; i += numOfThreads)
		    if (isPrime(arr[i]))
			   counters[threadId]++;
	} // end parallel
    
	int counter = 0;
	for (int i = 0; i < maxThreads; i++)
	    counter += counters[i];
	
	free(counters);

	return counter;
}

int countPrimesSeq(int* arr, int size)
{
	int count = 0;
	for (int i = 0; i < size; i++)
	{
			if (isPrime(arr[i]))
			    count++;
	}
    return count;
}

void generateNumbers(int* arr, int size)
{
#pragma omp parallel
	{
		int threadId = omp_get_thread_num();
		int numOfThreads = omp_get_num_threads();

        unsigned int seed = time(NULL) + threadId;
		// srand(time(NULL) + threadId); and then use rand(). But this
		// is not reentrant. rand_r(() is reentrant)

		for (int i = threadId; i < size; i += numOfThreads)
		{
			arr[i] = rand_r(&seed) % MAX_NUMBER; 
		}
	}
}
