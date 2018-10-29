#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>

unsigned long hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
} // http://www.cse.yorku.ca/~oz/hash.html

int comparePasswords(unsigned char *input, unsigned long passwordHash)
{
  return (hash(input) == passwordHash);
}

const int numList[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};


int main(int argc, char *argv[])
{
	const int MAX_PASS_LEN = 10;
	int rank, size, len, elementsPerProcess;
	char name[MPI_MAX_PROCESSOR_NAME];
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Get_processor_name(name, &len);
	
	elementsPerProcess = (sizeof(numList) / sizeof(numList[0])) / size;
	int i;
	
	int *sub_rand_nums = (int *)malloc(sizeof(int) * elementsPerProcess);
	
	MPI_Scatter(&numList, elementsPerProcess, MPI_INT, sub_rand_nums, elementsPerProcess, MPI_INT, 0, MPI_COMM_WORLD);
	for(i = 0; i < elementsPerProcess; i++)
	{
		printf("the elementsPerProcess is %d\n", elementsPerProcess);
		printf("I am node %d getting %d\n", rank, sub_rand_nums[i]);
	} // TEST FOR LOOP
	
	//printf ("Hello world! I'm %d of %d on %s\n", rank, size, name);
	MPI_Finalize();
	exit(0);
}