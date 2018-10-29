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

const char *passList[10] = {"password", "stuff", "door", "lamp", "secure", "12345", "mpi", "onomatopoeia", "thicc", "phone"};

int main(int argc, char *argv[])
{
	const int MAX_PASS_LEN = 10;
	int rank, size, elementsPerProcess;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	elementsPerProcess = (sizeof(passList) / sizeof(passList[0])) / size;
	char **subList;
	int i;
	
	subList = malloc(elementsPerProcess * sizeof(char*));
	for (i = 0; i < elementsPerProcess; i++)
	{
		subList[i] = malloc((MAX_PASS_LEN + 1) * sizeof(char));
	}
	
	MPI_Scatter(passList, elementsPerProcess, MPI_CHAR, subList, elementsPerProcess, MPI_CHAR, 0, MPI_COMM_WORLD);
	
	for(i = 0; i < elementsPerProcess; i++)
	{
		printf("I am node %d getting %s\n", rank, subList[i]);
	} // TEST FOR LOOP
	
	MPI_Finalize();
	exit(0);
}