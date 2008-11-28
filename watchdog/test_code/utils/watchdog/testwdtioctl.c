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
        int data = 0, data2 = 0;
        int ret_val;
        struct watchdog_info ident;

        int fd=open("/dev/watchdog",O_WRONLY);
        if (fd==-1) {
                perror("watchdog");
                return 1;
        }

	if (argc == 2)
		sscanf(argv[argc-1], "%d",&data2 );
	else 
       		data2 = 2;

        ret_val = ioctl (fd, WDIOC_GETSUPPORT, &ident);
        if (ret_val) {
	        printf ("\nWatchdog Timer : WDIOC_GETSUPPORT failed");
        }
        else {
	        printf ("\nIdentity : %s", ident.identity);
	        printf ("\nFirmware_version : %d", ident.firmware_version);
	        printf ("\nOptions : 0x%x", ident.options);
        }

        ret_val = ioctl (fd, WDIOC_GETSTATUS, &data);
        if (ret_val) {
	        printf ("\nWatchdog Timer : WDIOC_GETSTATUS failed");
        }
        else {                               
	        printf("\nget Status : %x", data);
        }


        ret_val = ioctl (fd, WDIOC_GETBOOTSTATUS, &data);
        if (ret_val) {
	        printf ("\nWatchdog Timer : WDIOC_GETBOOTSTATUS failed");
        }
        else {
	        printf ("\nBoot Status : %x", data);
        }

        ret_val = ioctl (fd, WDIOC_GETTIMEOUT, &data);
        if (ret_val) {
	        printf ("\nWatchdog Timer : WDIOC_GETTIMEOUT failed");
        }
        else {
	        printf ("\nCurrent timeout value before settime is : %d seconds\n", data);
        }

        ret_val = ioctl (fd, WDIOC_SETTIMEOUT, &data2);
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
                ret_val = ioctl (fd, WDIOC_KEEPALIVE, &data);
                if (ret_val) 
                {
                    printf ("\nWatchdog Timer : WDIOC_KEEPALIVE failed");
                }
                sleep(data-1);
        }
        close (fd);
	return 0;
}
