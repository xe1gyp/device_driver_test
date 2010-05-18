#include "common.h"

int writeport(int *fd, unsigned char *chars, int len) {
	int n = write(*fd, chars, len);
        if (n < 0) {
                fputs("write failed!\n", stderr);
                return 0;
        }
	/* printf("\n written %d bytes to port \n",n); */
        return SUCCESS;
}

int readport(int *fd, unsigned char *result) {
        int iIn=0;
	iIn = read(*fd, result, bufsize);
	if (iIn < 0) {
                if (errno == EAGAIN) {
                        printf("SERIAL EAGAIN ERROR\n");
                        return 0;
                } else {
                      printf("SERIAL read error %d %s\n", errno, strerror(errno));
                        return 0;
                }
        }
	/* printf("\n read %d bytes from port",iIn); */
	/* If we got an expected response trailer, break. */
	if(iIn ==1 && result[0] == 0x00 ) {
		/* printf("\n iIn ==1 and 0x00 character \n"); */
		return 0;
	}
        return iIn;
}

int getbaud(int fd) {
        struct termios termAttr;
        int Speed = -1;
        speed_t baudRate;
        tcgetattr(fd, &termAttr);
        /* Get the input speed.                              */
        baudRate = cfgetispeed(&termAttr);
	switch (baudRate) {
                case B300:    Speed = 300; break;
                case B600:    Speed = 600; break;
                case B1200:   Speed = 1200; break;
                case B1800:   Speed = 1800; break;
                case B2400:   Speed = 2400; break;
                case B4800:   Speed = 4800; break;
                case B9600:   Speed = 9600; break;
                case B19200:  Speed = 19200; break;
                case B38400:  Speed = 38400; break;
                case B115200: Speed = 115200; break;
                case B230400: Speed = 230400; break;
                case B460800: Speed = 460800; break;
                case B500000: Speed = 500000; break;
                case B576000: Speed = 576000; break;
                case B921600: Speed = 921600; break;
                case B1000000:Speed = 1000000; break;
                case B1152000:Speed = 1152000; break;
                case B1500000:Speed = 1500000; break;
                case B2000000:Speed = 2000000; break;
                case B2500000:Speed = 2500000; break;
                case B3000000:Speed = 3000000; break;
                case B3500000:Speed = 3500000; break;
        }
        return Speed;
}

int initport(int fd,long baudrate,int flow_ctrl)
{
	struct termios options;
        memset(&options, 0 , sizeof(options));
        if(ERROR == tcgetattr(fd, &options)) {
                perror("tcgetattr: ");
                return ERROR;
        }

	switch (baudrate) {
		case 300:	cfsetispeed(&options,B300);
				cfsetospeed(&options,B300);
				options.c_cflag |= B300 | CBAUD;
				break;
		case 600:	cfsetispeed(&options,B600);
				cfsetospeed(&options,B600);
				options.c_cflag |= B600 | CBAUD;
				break;
		case 1200:	cfsetispeed(&options,B1200);
				cfsetospeed(&options,B1200);
				options.c_cflag |= B1200 | CBAUD;
				break;
		case 1800:	cfsetispeed(&options,B1800);
				cfsetospeed(&options,B1800);
				options.c_cflag |= B1800 | CBAUD;
				break;
		case 2400:	cfsetispeed(&options,B2400);
				cfsetospeed(&options,B2400);
				options.c_cflag |= B2400 | CBAUD;
				break;
		case 4800:	cfsetispeed(&options,B4800);
				cfsetospeed(&options,B4800);
				options.c_cflag |= B4800 | CBAUD;
				break;
		case 9600:
				cfsetispeed(&options,B9600);
				cfsetospeed(&options,B9600);
				options.c_cflag |= B9600 | CBAUD;
				break;
		case 19200:	cfsetispeed(&options,B19200);
				cfsetospeed(&options,B19200);
				options.c_cflag |= B19200 | CBAUD;
				break;
		case 38400:	cfsetispeed(&options,B38400);
				cfsetospeed(&options,B38400);
				options.c_cflag |= B38400 | CBAUD;
				break;
		case 115200:	cfsetispeed(&options,B115200);
				cfsetospeed(&options,B115200);
				options.c_cflag |= B115200 | CBAUDEX;
				break;
		case 230400:	cfsetispeed(&options,B230400);
				cfsetospeed(&options,B230400);
				options.c_cflag |= B230400 | CBAUDEX;
				break;
		case 460800:	cfsetispeed(&options,B460800);
				cfsetospeed(&options,B460800);
				options.c_cflag |= B460800 | CBAUDEX;
				break;
		case 500000:	cfsetispeed(&options,B500000);
				cfsetospeed(&options,B500000);
				options.c_cflag |= B500000 | CBAUDEX;
				break;
		case 576000:	cfsetispeed(&options,B576000);
				cfsetospeed(&options,B576000);
				options.c_cflag |= B576000 | CBAUDEX;
				break;
		case 921600:	cfsetispeed(&options,B921600);
				cfsetospeed(&options,B921600);
				options.c_cflag |= B921600 | CBAUDEX;
				break;
		case 1000000:	cfsetispeed(&options,B1000000);
				cfsetospeed(&options,B1000000);
				options.c_cflag |= B1000000 | CBAUDEX;
				break;
		case 1152000:	cfsetispeed(&options,B1152000);
				cfsetospeed(&options,B1152000);
				options.c_cflag |= B1152000 | CBAUDEX;
				break;
		case 1500000:	cfsetispeed(&options,B1500000);
				cfsetospeed(&options,B1500000);
				options.c_cflag |= B1500000 | CBAUDEX;
				break;
		case 2000000:	cfsetispeed(&options,B2000000);
				cfsetospeed(&options,B2000000);
				options.c_cflag |= B2000000 | CBAUDEX;
				break;
		case 2500000:	cfsetispeed(&options,B2500000);
				cfsetospeed(&options,B2500000);
				options.c_cflag |= B2500000 | CBAUDEX;
				break;
		case 3000000:	cfsetispeed(&options,B3000000);
				cfsetospeed(&options,B3000000);
				options.c_cflag |= B3000000 | CBAUDEX;
				break;
		case 3500000:	cfsetispeed(&options,B3500000);
				cfsetospeed(&options,B3500000);
				options.c_cflag |= B3500000 | CBAUDEX;
				break;
		default:
				printf("\n Provided an Invalid Baudrate.\n Proceeding using default baud 115200\n");
				cfsetispeed(&options,B115200);
				cfsetospeed(&options,B115200);
				options.c_cflag |= B115200 | CBAUDEX;
				break;
	}

	/* Enable or Disable Hardware flow control. */
        if(!flow_ctrl) {
		options.c_cflag &= ~CRTSCTS;
		options.c_iflag &= ~(IXON | IXOFF | IXANY);
		printf("\n Flow control disabled \n");
	} else if(flow_ctrl == 1) {
		options.c_cflag |= CRTSCTS;
		options.c_iflag &= ~(IXON | IXOFF | IXANY);
		printf("\n Hardware FlowControl: Enabling RTS/CTS support. \n");
	} else if(flow_ctrl == 2) {
		options.c_cflag &= ~CRTSCTS;
		options.c_iflag |= (IXON | IXOFF | IXANY);
		/* XOFF - Pause transmission  - DC3 - 0x13  */
		/* XON  - Resume transmission -	DC1 - 0x11  */
		options.c_cc[VSTART] = 0x11;
		options.c_cc[VSTOP] = 0x13;
		printf("\n Software FlowControl: Enabling XON/XOFF support. \n");
	}
        /* No Parity, 8N1. */
        options.c_cflag &= ~PARENB; /* No parity bit 	*/
        options.c_cflag &= ~CSTOPB; /* 1 stop bit 	*/
        options.c_cflag &= ~CSIZE;  /* character size 8 */
        options.c_cflag |=  CS8 ;

	/*
	printf("\n  options.c_iflag = 0x%x \n", options.c_iflag);
	*/

        /* Hardware Control Options - Set local mode and Enable receiver to receive characters */
        options.c_cflag     |= (CLOCAL | CREAD );

        /* Terminal Control options */
        options.c_lflag     &= ~(ICANON | IEXTEN | ECHO | ISIG); /* Line options - Raw input */

	options.c_iflag     &= ~(ICRNL | INPCK | ISTRIP | BRKINT);
        /* Output processing - Disable post processing of output. */
        options.c_oflag     &= ~OPOST;      /* Output options - Raw output */
        /* Control Characters - Min. no. of characters */
        options.c_cc[VMIN]  = 0;
        /* Character/Packet timeouts. */
        options.c_cc[VTIME] = 3;

	if(ERROR == tcflush(fd,TCIOFLUSH) ) {
                  printf("\n Flushing port failed");
                  return ERROR;
        }

	if(ERROR == tcsetattr(fd, TCSANOW, &options))  {
		printf("\n Error: Couldn't configure Serial port");
		return ERROR;
        }
/*
	printf("\n\n");
	system("stty -F /dev/ttyS0 -a");
	printf("\n\n");
*/
	return SUCCESS;
}


void signalHandler()
{
	printf("\n Closing device\n");
	gettimeofday(&ut.end_time,NULL);
	timersub(&ut.end_time,&ut.start_time,&ut.diff_time);

	if(read_flag && tx_rx == 'r') {
	        ut.diff_time.tv_sec -= 3;
	        printf("\n Time taken %08ld sec, %08ld usec\n\n ",ut.diff_time.tv_sec,ut.diff_time.tv_usec);
	}
	if(tx_rx == 's')
	        printf("\n Time taken %08ld sec, %08ld usec\n\n ",ut.diff_time.tv_sec,ut.diff_time.tv_usec);

	close_port();
}

int create_sample_send_file(char *argv)
{
	int i;
	int fds;
	char known_pattern[] = {0xAA};

	sprintf(tx_rx_filename, "uart_tx_file_%s", argv);
	fds = open(tx_rx_filename, O_RDWR|O_CREAT|O_TRUNC, 0666);
	if (fds == ERROR)
		return ERROR;

	for (i = 0 ; i < MB; i++)
		write(fds, known_pattern, sizeof(known_pattern));

	lseek(fds, 0, SEEK_SET);
	return fds;
}

void close_port()
{
        tcsetattr(ut.fd, TCSANOW, &oldtio);
	if(tx_rx == 'r')
	        close(fd2);
	else
		close(fd1);
        close(ut.fd);
        exit(1);
}

void display_intro()
{
	printf("\nUse the following format to run the HS-UART TEST PROGRAM \n");
	printf("\nFor sending data: \n./ts_uart s <tty_interface> <baudrate> <flow_control(0/1/2)>\n");
	printf("\nFlow control bits: \n"
			"0: Using No flow control. \n"
			"1: Hardware FlowControl: To Enable RTS/CTS support.\n"
			"2: Software FlowControl: To Enable XON/XOFF support.\n");
	printf("\n Ex. ./ts_uart s ttyO1 115200 1 \n");
	printf("\n For Receiving data:");
	printf("\n Ex. ./ts_uart r ttyO1 115200 1 \n");
}

