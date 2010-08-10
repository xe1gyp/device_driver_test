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

int main(int argc, char *argv[]){

	struct input_event{
	       	struct timeval time;
	        unsigned short type;
	        unsigned short code;
	        unsigned int value;
	}keyinfo;
	
	int bytes;
	int fd = open("/dev/input/event0",O_RDONLY);

	if(fd > 0){
		if(argc > 1) {
			printf("\n\n\t%s \n\n", argv[1]);	
			fflush(stdout);
		}
		else {
			printf("\n\n\t Press any key: \n\n" );
			fflush(stdout);
		}
		while(1){
			bytes = read(fd, &keyinfo, sizeof(struct input_event));
			if(bytes && keyinfo.type==0x01){
				if (keyinfo.code != 0){ 
					close(fd);
					break;
				}
			}
		}
	}
	
	return 0;
}

