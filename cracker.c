#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <unistd.h>
#define MASTER 0

unsigned long hash(unsigned char *str) {
	unsigned long hash = 5381;
	int c;

	while ((c = *str++)) {
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}

	return hash;
} // http://www.cse.yorku.ca/~oz/hash.html

int comparePasswords(unsigned char *input, unsigned long passwordHash) {
	return (hash(input) == passwordHash);
}

char **passList, **subList;
unsigned long hashToFind;
int i, j, rank, size, elementsPerProcess, arraySize, MAX_PASS_LEN, sendExit,
		exitCode, flag;
MPI_Status status;
MPI_Status status2;
MPI_Request request;

void init_system(int argc, char *argv[]);

void root_task();

void worker_task();

int main(int argc, char *argv[]) {

	init_system(argc, argv);

	if (rank == MASTER) {
		root_task();
	} else {
		worker_task();
	}
	MPI_Finalize();
	exit(0);
}

void init_system(int argc, char *argv[]) {

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	exitCode = 0;
	sendExit = 1;
	flag = 0;

	FILE *input;
	//read file and get array size
	hashToFind = strtoul(argv[2], NULL, 0);
	input = fopen(argv[1], "r");
	int i = 0;
	MAX_PASS_LEN = 0;
	while (1) {
		char ch = (char) fgetc(input);
		j = 0;
		while (ch != '\n' && !feof(input)) {    //read till \n or EOF
			j++;
			ch = (char) fgetc(input);
		}
		if (feof(input)) {        //check again for EOF
			break;
		}
		if (j > MAX_PASS_LEN)
			MAX_PASS_LEN = j;
		i++;
	}
	fclose(input);

	arraySize = i;
	elementsPerProcess = arraySize / size;
	if (rank == size - 1) { // Last node
		elementsPerProcess += arraySize % size;
	}
	if (rank == MASTER) {
		passList = (char **) malloc(sizeof(char *) * arraySize);
		for (i = 0; i < arraySize; i++) {
			passList[i] = (char *) malloc((MAX_PASS_LEN) * sizeof(char));
			memset(passList[i], '\0', (MAX_PASS_LEN) * sizeof(char));
		}
		i = 0; // reset variable to 0
		// read file and save to array
		input = fopen(argv[1], "r");
		while (1) {
			char ch = (char) fgetc(input);
			j = 0;
			while (ch != '\n' && !feof(input)) {    //read till \n or EOF
				passList[i][j++] = ch;            //store in array
				ch = (char) fgetc(input);
			}
			if (feof(input)) {        //check again for EOF
				break;
			}
			i++;
		}
	}
	subList = (char **) malloc(sizeof(char *) * elementsPerProcess);
	for (i = 0; i < elementsPerProcess; i++) {
		subList[i] = (char *) malloc((MAX_PASS_LEN) * sizeof(char));
		memset(subList[i], '\0', (MAX_PASS_LEN) * sizeof(char));
	}
}

void root_task() {
	j = elementsPerProcess;
	int k = j;
	for (i = 1; i < size; i++) {
		if (i == size - 1) { // Last node
			elementsPerProcess += arraySize % size;
		}
		for (; j < k + elementsPerProcess; j++) {
			MPI_Send(passList[j], (int) strlen(passList[j]), MPI_CHAR, i, 0,
					MPI_COMM_WORLD);
		}
		k = j;
	}
	MPI_Irecv(&exitCode, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD,
			&request);
	for (i = 0; i < elementsPerProcess; i++) {
		MPI_Test(&request, &flag, &status2);
		if (flag != 0) {
			//printf("Node %d: Received EXIT\n", rank);
			break;
		}
		if (comparePasswords((unsigned char*) passList[i], hashToFind)) {
			printf("The password is %s\n", passList[i]);
			MPI_Cancel(&request);
			for (i = 1; i < size; i++) {
				//printf("Sending exit to %d\n", i);
				MPI_Send(&sendExit, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
			}
			break;
		}
	}
}

void worker_task() {
	for (i = 0; i < elementsPerProcess; i++) {
		MPI_Recv(subList[i], MAX_PASS_LEN, MPI_CHAR, MASTER, 0, MPI_COMM_WORLD,
				&status);
	}
	MPI_Irecv(&exitCode, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD,
			&request);
	for (i = 0; i < elementsPerProcess; i++) {
		MPI_Test(&request, &flag, &status2);
		if (flag != 0) {
			//printf("Node %d: Received EXIT\n", rank);
			break;
		}
		if (comparePasswords((unsigned char*) subList[i], hashToFind)) {
			printf("The password is %s\n", subList[i]);
			MPI_Cancel(&request);
			for (i = 0; i < size; i++) {
				if (i != rank) {
					//printf("Sending exit to %d\n", i);
					MPI_Send(&sendExit, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
				}
			}
			break;
		}
	}
}
