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

void arrayProcessing(char[] *input, unsigned long passwordHash)
{
  int inputSize = sizeof(input)/sizeof(unsigned char);
  int isPassword = 0; // 0 for false, 1 for true
  int i = 0;
  for(i = 0; i < inputSize; i++)
  {
    isPassword = comparePasswords(input[i], passwordHash);
    if(isPassword) // String is the password
    {
      char 
      //not entirely sure what to put here when you've obtained the password
      //MPI_SEND();
      //break;
    }
  }
}

const char *passList[10] = {"password", "stuff", "door", "lamp", "secure", "12345", "mpi", "onomatopoeia", "thicc", "phone"};


int main(int argc, char *argv[])
{
	const int MAX_PASS_LEN = 10;
	int rank, size, len, elementsPerProcess;
	char name[MPI_MAX_PROCESSOR_NAME];
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Get_processor_name(name, &len);
	printf("Stuff\n");
	elementsPerProcess = size / (sizeof(passList) / sizeof(passList[0]));
	char **subList;
	int i;
	
	subList = malloc(elementsPerProcess * sizeof(char*));
  
	for (i = 0; i < elementsPerProcess; i++)
  {
    subList[i] = malloc((MAX_PASS_LEN + 1) * sizeof(char));
  }
  
  MPI_Scatter(passList, elementsPerProcess, MPI_CHAR, subList, elementsPerProcess, MPI_CHAR, 0, MPI_COMM_WORLD);
  
  for(i = 0; i < 2; i++)
  {
		printf("I am node %d getting %s\n", rank, *subList[i]);
  } // TEST FOR LOOP
	printf ("Hello world! I'm %d of %d on %s\n", rank, size, name);
	MPI_Finalize();
	exit(0);
}



// Reading text from file to array
#include <stdio.h>

int main(int argc, char const *argv[])
{
    FILE* inp;
    inp = fopen("800k.txt","r");
    char arr[800000][10];
    int i = 0;
    while(1){
        char r = (char)fgetc(inp);
        int k = 0;
        while(r!='\n' && !feof(inp)){	//read till \n or EOF
            arr[i][k++] = r;			//store in array
            r = (char)fgetc(inp);
        }
        arr[i][k]=0;		//make last character of string null
        if(feof(inp)){		//check again for EOF
            break;
        }
        i++;
    }
    int j;
    for(j = 0;j<=i;j++){
        printf("%s\n",arr[j] );	//print array
    }
    return 0;
}

MPI_Scatter(*arr, (i+1)/size, MPI_Char,
               workerArr, (i+1)/size, MPI_Char, 0,
               MPI_World_Comm)
char workerArr[(i+1)/size][10];
// Root node does above, i+1 is how many passwords read, so i/numberofworkernodes is how many pw to allocate to scatter around