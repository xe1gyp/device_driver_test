/* Test Program for the Watchdog Timer driver */
/* 19.02.2004  */

#include <stdio.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <asm/types.h>
#include <linux/watchdog.h>
#include <sys/stat.h>

int main(int argc, const char *argv[]) {
        int data = 0;
        int ret_val;

        int fd=open("/dev/watchdog",O_WRONLY);
        if (fd==-1) {
                perror("watchdog");
                return 1;
        }

	data = 2;
        ret_val = ioctl (fd, WDIOC_SETTIMEOUT, &data);
        if (ret_val) {
	        printf ("\nWatchdog Timer : WDIOC_SETTIMEOUT failed");
        }
        else {
	        printf ("\nNew timeout value is : %d seconds", data);
        }

        ret_val = ioctl (fd, WDIOC_GETTIMEOUT, &data);
        if (ret_val) {
	        printf ("\nWatchdog Timer : WDIOC_GETTIMEOUT failed");
        }
        else {
	        printf ("\nCurrent timeout value is : %d seconds\n", data);
        }

        while(1) 
        {
                write(fd, "\0", 1);
                sleep(1);
        }
        close (fd);
	
	return 0;
}
