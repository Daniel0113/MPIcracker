#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <unistd.h>

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
int i, j, rank, size, elementsPerProcess, arraysize, MAX_PASS_LEN;
MPI_Status status;

void init_system(int argc, char *argv[]);

void root_task();

void worker_task();

int main(int argc, char *argv[]) {

    init_system(argc, argv);

    if (rank == 0) {
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

    FILE *inp;
    //read file and get array size
    hashToFind = strtoul(argv[2], NULL, 0);
    inp = fopen(argv[1], "r");
    int i = 0;
    MAX_PASS_LEN = 0;
    while (1) {
        char r = (char) fgetc(inp);
        int k = 0;
        while (r != '\n' && !feof(inp)) {    //read till \n or EOF
            k++;
            r = (char) fgetc(inp);
        }
        if (feof(inp)) {        //check again for EOF
            break;
        }
        if (k > MAX_PASS_LEN) MAX_PASS_LEN = k;
        i++;
    }
    fclose(inp);

    arraysize = i;
    if (arraysize % size != 0) {
        printf("File must be divisible by nodes\n");
        MPI_Abort(MPI_COMM_WORLD, 2);
    }
    elementsPerProcess = arraysize / size;

    if (rank == 0) {
        passList = (char **) malloc(sizeof(char *) * arraysize);
        for (i = 0; i < arraysize; i++) {
            passList[i] = (char *) malloc((MAX_PASS_LEN) * sizeof(char));
            memset(passList[i], '\0', (MAX_PASS_LEN) * sizeof(char));
        }
        i = 0;
        // read file and save to array
        inp = fopen(argv[1], "r");
        while (1) {
            char r = (char) fgetc(inp);
            int k = 0;
            while (r != '\n' && !feof(inp)) {    //read till \n or EOF
                passList[i][k++] = r;            //store in array
                r = (char) fgetc(inp);
            }
            if (feof(inp)) {        //check again for EOF
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
        for (; j < k + elementsPerProcess; j++) {
            MPI_Send(passList[j], strlen(passList[j]), MPI_CHAR, i, 0, MPI_COMM_WORLD);
        }
        k = j;
    }

    for (i = 0; i < elementsPerProcess; i++) {
        if(comparePasswords(passList[i], hashToFind)){
            printf("I am node %d. I have %s\n", rank, passList[i]);
        }
    }
}

void worker_task() {
    for (i = 0; i < elementsPerProcess; i++) {
        MPI_Recv(subList[i], MAX_PASS_LEN, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
    }
    for (i = 0; i < elementsPerProcess; i++) {
        if(comparePasswords(subList[i], hashToFind)){
            printf("I am node %d. I have %s\n", rank, subList[i]);
        }
    }
}