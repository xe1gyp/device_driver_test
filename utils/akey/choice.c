#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/kernel.h>
#include <linux/version.h>

/*
 * to run this program we have to choose Event interface under 
 * Input device support when  make menuconfig
 *
*/

int main(void){

	struct input_event{
	       	struct timeval time;
	        unsigned short type;
	        unsigned short code;
	        unsigned int value;
	}keyinfo;
	
	int bytes;
	int fd = open("/dev/input/event0",O_RDONLY);
	int ret;
	
	if(fd > 0){
		sleep(1);
		printf("Press a key:\nPASS (S26 || S5)\nFAIL (S19 || S11)\n");
		fflush(stdout);
		while(1){
			bytes = read(fd, &keyinfo, sizeof(struct input_event));
			if(bytes && keyinfo.type==0x01){
				if (keyinfo.code == 35 || keyinfo.code == 2){ 
					printf("Validation Criteria: PASS\n");
					fflush(stdout);
					ret = 0;
          close(fd);
					break;
				}
				if (keyinfo.code == 22 || keyinfo.code == 5){
					printf("Validation Criteria: FAIL\n");
					fflush(stdout);
					ret = 1;
          close(fd);
					break;
				}
			}
		}
	}
	
	return ret;
}

