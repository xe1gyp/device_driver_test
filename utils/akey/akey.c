#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/input.h>

/*
 * to run this program we have to choose Event interface under 
 * Input device support when  make menuconfig
 *
*/
int main(int argc, char *argv[])
{
	char *input_device = "/dev/input/event0";

	if (argc > 1) {
		int size = (strlen(argv[1]) + 1) * sizeof(char);
		input_device = (char *) malloc(size);
		strcpy(input_device, argv[1]);
	}

	struct input_event keyinfo;
	int bytes;
	int fd = open(input_device, O_RDONLY);
	int ret;
	
	if (fd > 0) {
		sleep(1);
		printf("Press a key:\n"
			"PASS (3430 & 3630 SDP: S26 ||"
			" Zoom2/3 & SDP4430: H or Send)\n"
			"FAIL (3430 & 3630 SDP: S19 ||"
			" Zoom2/3 & SDP4430: U or End)\n");
		fflush(stdout);
		while(1) {
			bytes = read(fd, &keyinfo, sizeof(keyinfo));
			if (bytes && keyinfo.type == EV_KEY) {
				if (keyinfo.code == KEY_H ||
				    keyinfo.code == KEY_SEND) {
					printf("Validation Criteria: PASS\n");
					fflush(stdout);
					ret = 0;
					break;
				}
				if (keyinfo.code == KEY_U ||
				    keyinfo.code == KEY_END) {
					printf("Validation Criteria: FAIL\n");
					fflush(stdout);
					ret = 1;
					break;
				}
			}
		}
		close(fd);
	}
	
	return ret;
}

