#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAXLINELENGTH 1024
#define BUFSIZE 500


long process_data(char buf[BUFSIZE], long bytesread, int loop)
{
	char *lines = malloc(sizeof(char)*bytesread);
	char
}


int main(int argc, char **argv){

	long bytesread;
	char buf[BUFSIZE];
	int sizeLeftOver = 0;
	int bLoopCompleted = 0;
	long pos = 0;
	FILE *handle;

	if (!(handle = fopen(argv[1],"rb")))
		exit(1);

	bytesread = fread(buf+sizeLeftOver, 1, sizeof(buf)-1-sizeLeftOver,handle);

	if(bytesread<1){
		bLoopCompleted = 1;
		bytesread = 0;
	}

	buf[bytesread+sizeLeftOver] = 0;

	fclose(handle);
	return 0;
}