#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

char **passList;
unsigned long hashToFind;
int i, j, arraySize, MAX_PASS_LEN, exitCode;

void init_system(int argc, char *argv[]);

void root_task();

void worker_task();

int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Usage: ./cracker <wordlist> <hash>\n");
		exit(1);
	}
	init_system(argc, argv);

	root_task();
	if (exitCode != 1)
		printf("Password was not found.\n");
	exit(0);
}

void init_system(int argc, char *argv[]) {

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

void root_task() {
	for (i = 0; i < arraySize; i++) {
		if (comparePasswords((unsigned char*) passList[i], hashToFind)) {
			printf("The password is %s\n", passList[i]);
			exitCode = 1;
			break;
		}
	}
}
