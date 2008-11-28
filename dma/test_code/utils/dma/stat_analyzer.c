#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(argc, argv)
int argc; char *argv[];
{
	FILE *fp;
	char *filename;
	int ret = 0;
	int chan, xfers, good, max; 
	char status[5] = {0};

	if (argc == 2){
		filename = (char *)malloc(strlen(argv[1])+1);
		strcpy(filename, argv[1]);
		printf("Analyzing test results file: %s\n", filename);
	} else {
		printf("No file specified. The file to be parsed needs to be passed as an argument!\n");
		exit(1);
	}

	fp = fopen(filename, "r");
	if (fp == NULL){
		printf("Incorrect filename or path!!!\n");
		exit(1);
	}

	while(!feof(fp)){
		fscanf(fp, "%d %d %d %d\n", &chan, &xfers, &good, &max);
		if (xfers != good){
			strcpy(status, "FAIL");
			ret++;
		} else {
			strcpy(status, "PASS");
		}

		printf("Channel=%d Xfers=%d Good=%d Max=%d Status=%s\n", chan, xfers, good, max, status);
	}

	
	fclose(fp);

	return ret;
}
