#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "/vobs/wtbu/CSSD_L_GIT_2.6/linux/kernel_org/2.6_kernel/include/linux/usb/iccd_ioctl.h"


main()
{
	int i, fd;
	int size = 0, ret;
	unsigned char resp[50];
		/* APDU: Select Main File */
	unsigned char selectMF[7] =
		{ 0xA0, 0xA4, 0x00, 0x00, 0x02, 0x3F, 0x00  };

		/* APDU: Get Response */
	unsigned char get_resp[5] =
		{ 0xA0, 0xC0, 0x00, 0x00, 0x00 };

	fd = open("/dev/sim0", O_RDWR|O_NONBLOCK, 0);
	if (-1 == fd)
		printf("\n error Open()\n");

	/* Send: PowerOn Message --> and wait for ATR */
	printf("\nTest 1: -->\n PowerOn ICCD");
	ret = ioctl(fd, ICCD_IOC_POWERON, NULL);
	if (ret < 0) {
		printf("\n Cannot PowerOn device. Exiting... \n");
		close(fd);
		return 0;
	}

	size = read(fd, resp, 50);
	if (size < 0) {
		printf("\n Wrong ATR. Exiting... \n");
		close(fd);
		return 0;
	}
	printf("\n Response [%d] ATR[ ", size);
	/* Print ATR */
	for (i = 0; size && i < size; i++)
		printf("%0x ", resp[i]);
	printf("]\n\n");


	/* Select MF: APDU test */
	printf("\n\nTest 2: -->\n Select MF[ ");
	for (i = 0; i < sizeof(selectMF); i++)
		printf("%0x ", selectMF[i]);
	printf("]");
	size = write(fd, selectMF, 7);
	size = read(fd, resp, 50);
	printf("\n Response [%d] [ ", size);
	for (i = 0; i < size; i++)
		printf("%0x ", resp[i]);
	printf("]");

	/* Get Response */
	get_resp[4] = resp[size-1];
	printf("\n Get-Response [ ");
	for (i = 0; i < sizeof(get_resp); i++)
		printf("%0x ", get_resp[i]);
	printf("]");

	size = write(fd, get_resp, 5);
	size = read(fd, resp, 50);
	printf("\n Response [%d] [", size);
	for (i = 0; i < size; i++)
		printf("%0x ", resp[i]);
	printf("]\n");

	close(fd);
	printf("\n\n\n\n");
}
