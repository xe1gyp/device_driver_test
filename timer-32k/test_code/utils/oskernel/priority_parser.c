#include <stdio.h>

#define PRIORITIES_NUM 3

int main(void){
	FILE *Fp;
	int priorities[2*PRIORITIES_NUM] = {0};
	int i;
	int ret = 0;

	/* Get values read from file */
	Fp = fopen("/pri_int.msg", "r"); 

	for(i=0; i<2*PRIORITIES_NUM; i++){
		fscanf(Fp, "%d\n", priorities+i);
	}

	fclose(Fp);
	
	/* Parse the data read and verify the validation criteria */
	for (i=0; i<PRIORITIES_NUM; i++){
		printf("Parsing priorities[%d]=%d and priorities[%d]=%d : ", i, priorities[i], 2*PRIORITIES_NUM-1-i, priorities[2*PRIORITIES_NUM-1-i]);

		if (priorities[i] != priorities[2*PRIORITIES_NUM-1-i]){
			printf("FAIL\n");
			printf("Validation criteria is not met!\n");
			ret = 1;
			break;
		}
		printf("PASS\n");
	}

	return ret;
}

