#include <stdio.h>	/* Standard input/output definitions */
#include <string.h>	/* String function definitions */
#include <unistd.h>	/* UNIX standard function definitions */
#include <fcntl.h>	/* File control definitions */
#include <sys/time.h>	/* To calulate time taken for operaton */
#include <errno.h>	/* Error number definitions */
#include <termios.h>	/* terminal control definitions */
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/ioctl.h>

#define ERROR -1
#define SUCCESS 1

#define UART_DEV_NAME   "/dev/ttyS0"

static int bufsize = 4096 ;	/* buffer of size 4K */
/*static int bufsize = 2048;	buffer of size 4K */

/* contains varibles required for UART PORT TESTING */
struct uart_test {
	int		fd;
	long int	baudrate;
	int		flow_cntrl;
	struct timeval start_time, end_time, diff_time;
};

/**
* Structure used to store existing port informatiom
* before we re-configure the port as per our testing
* requirements.
*/
struct termios oldtio;

int fd1,fd2;
struct uart_test ut;
char tx_rx;
int read_flag;
FILE *md5_fd;

int writeport(int *fd, unsigned char *chars,int len);

int readport(int *fd, unsigned char *result);

int getbaud(int fd);

long int getbaud_flag(long int baudrate);

int initport(int fd,long int baudrate,int flow_ctrl);

void close_port();

void display_intro();

void signalHandler();
