#include <stdio.h>

#define VALUES_NUM 20

int main(void){
	FILE *Fp;
	int timer_val[VALUES_NUM] = {0};
	int i;
	int ret = 0;

	/* Get values read from file */
	Fp = fopen("/timer.msg", "r"); 

	for(i=0; i<VALUES_NUM; i++){
		fscanf(Fp, "%x\n", timer_val+i);
	}

	fclose(Fp);
	
	/* Parse the data read and verify the validation criteria */
	/* Validation criteria: timer_val[i] < timer_val[i+1] */
	for (i=0; i<VALUES_NUM-1; i++){
		printf("Parsing timer_val[%d]=%x and timer_val[%d]=%x : ", i, timer_val[i], i+1, timer_val[i+1]);
		if (timer_val[i] >= timer_val[i+1]){
			printf("FAIL\n");
			printf("Validation criteria is not met!\n");
			ret = 1;
			break;
		}
		printf("PASS\n");
	}

	return ret;
}

