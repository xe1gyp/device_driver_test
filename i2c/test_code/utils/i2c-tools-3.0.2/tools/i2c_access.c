/*
* Simple user application to read and write on I2C bus.
*
* Lesly A M <x0080970@ti.com>
*/

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>

#define I2C_1 "/dev/i2c-1"
#define I2C_2 "/dev/i2c-2"

int main(int argc, char *argv[])
{
	int fd;
	int ret = 0;
	unsigned int cmd;  /* Register addr */
	unsigned int value;  /* Value to write*/
	unsigned int slave_addr; /* sl addr on I2C_1: 0x48, 0x49, 0x4A,
0x4B(Power) */

	fd = open(I2C_1, O_RDWR);
	if (fd < 0) {
		perror("\nFailed to open I2C_1");
		exit(1);
	}

	 if ((argc < 3) || (argc > 4)) {
		printf("Usage: i2c_write <SLAVE ID> <REG ADD> <opt: VALUE>\n");
		exit(1);
	}

	slave_addr = strtol(argv[1], NULL, 0);
	cmd = strtol(argv[2], NULL, 0);
	if (argc == 4)
		value = strtol(argv[3], NULL, 0);

	if (ioctl(fd, I2C_SLAVE_FORCE, slave_addr) < 0) {
		perror("\nFailed to set slave address");
		exit(2);
	}

	if (argc == 4) {
		if (i2c_smbus_write_byte_data(fd, cmd, value) < 0)
			perror("\nI2C write failed");
	} else if (argc == 3) {
		ret = i2c_smbus_read_byte_data(fd, cmd);
		if (ret == -1)
			perror("\nI2C read failed");
		else
			printf("%02X\n", ret);
	}

	close(fd);
	return 0;
}
