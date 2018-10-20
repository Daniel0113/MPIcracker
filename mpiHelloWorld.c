#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>
int main(int argc, char *argv[])
{
	int rank, processes, source, dest;
	int tag = 50;
	char message[100];
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &processes);
	
	if(rank != 0) 
	{ 
		sprintf(message, "Greetings from process %d", rank);
		dest = 0; // this sets the destination to the master node
		MPI_Send(message, strlen(message) + 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
	}
	else // If we are the master node
	{
		for(source = 1; source < processes; source++)
		{
			MPI_Recv(message, 100, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
			printf("%s\n", message);
		}
	}
	
	MPI_Finalize();
	exit(0);
}