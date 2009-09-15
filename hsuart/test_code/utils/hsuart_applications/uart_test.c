#include "common.h"

int main(int argc, char **argv) {

        int i=0,rd=0;
        fd_set readFds;
        int retval;
        struct timeval respTime;
	unsigned char input_buf[bufsize],output_buf[bufsize];
	char tx_rx;
	int size=0;

	int read_flag=0;

	int j,error=0;

	if (argc !=5) {
//	printf("\n argc \n");
		display_intro();
		exit(1);
	}

	j=sscanf(argv[3],"%li",&ut.baudrate);
	if (j != 1) error=1;
	j=sscanf(argv[4],"%i",&ut.flow_cntrl);
	if (j != 1) error=1;
	if (error){
		printf("\n Invalid command line argument [argv] \n");
                display_intro();
                exit(1);
        }

	ut.fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK );

	if (ut.fd == -1) {
                perror("open_port: Unable to open /dev/ttyS2 - ");
		  exit(1);
        } else {
                fcntl(ut.fd, F_SETFL, 0);
        }

	printf("\n Existing port baud=%d", getbaud(ut.fd));
        tcgetattr(ut.fd,&oldtio); // save current port settings
        initport(ut.fd,ut.baudrate,ut.flow_cntrl);
        printf("\n Configured port for New baud=%d", getbaud(ut.fd));

	sscanf(argv[1],"%c",&tx_rx);

	switch (tx_rx) {

		case 'r':
			fd2 = open(argv[2],O_WRONLY|O_CREAT,S_IRWXU|S_IRWXO);
		        if (fd2 == -1) {
			        printf("\n cannot open %s \n",argv[2]);
	                close(ut.fd);
			exit(1);
			}
			while(1) {
				FD_ZERO(&readFds);
				FD_SET(ut.fd, &readFds);
				respTime.tv_sec =  10;
		                respTime.tv_usec = 0;

		                retval = select(FD_SETSIZE, &readFds, NULL, NULL, &respTime);

			      /* Sleep for command timeout and expect the response to be ready. */
				if(retval == ERROR)
	                                printf("\n select: error :: %d\n",retval);

			        /* Read data, abort if read fails. */
			        if(FD_ISSET(ut.fd, &readFds)!=0)  {
				        //fcntl(ut.fd, F_SETFL, FNDELAY);
				//	printf("\n entering readport func \n");
					if(read_flag == 0) {
						gettimeofday(&ut.tx_start_time);
						read_flag=1;
					}
					rd=readport(&ut.fd,output_buf);
					if ( ERROR == rd  ) {
				                printf("Read Port failed\n");
						close_port();
					}
				}
				if(read_flag == 0) {
						if (unlink(argv[2]) == -1)
						printf("\n Failed to delete the file %s \n",argv[2]);
						printf("\n waited for 10 seconds no data was available to read exiting \n");
						close_port();
						exit(0);
				}
				if( rd == 0 )
					break;
		                size+=rd;
			        i = write(fd2,&output_buf,rd);
				memset(output_buf,0,bufsize);
				//printf("\nport returned %d bytes Written %d bytes to output file",rd,i);
			}
			gettimeofday(&ut.tx_end_time);
			printf("\n Read %d bytes from port \n",size);
			FD_CLR(ut.fd, &readFds);
			close(fd2);
			break;

		case 's':
			fd1 = open(argv[2],O_RDONLY);
		        if(fd1 == -1 )  {
				printf("\n cannot open %s \n",argv[2]);
		                close(ut.fd);
				exit(1);
		        }
			gettimeofday(&ut.tx_start_time);
		        while(1) {
		                rd= read(fd1,&input_buf,bufsize-1);
			        if(rd == 0 )
				         break;
				size+=rd;
				//printf("\n Read from input file %d bytes \n",rd);
			        //input_buf[rd] = 0x00;
		                fcntl(ut.fd, F_SETFL, 0);
			        if (!writeport(&ut.fd, input_buf,rd)) {
				        printf("\n Writing to port failed\n");
					close_port();
				 }
		                 if(ERROR == tcdrain(ut.fd))
					printf("\n tcdrain failure \n");
				 memset(input_buf,0,bufsize);
			}
			gettimeofday(&ut.tx_end_time);
			printf("\n Written %d bytes from port \n",size);
			/* Wait for 5 seconds for Transmitting to complete before sending the Break sequence */
			sleep(5);
			for(i=0;i<5;i++)
			tcsendbreak(ut.fd,5);
			close(fd1);
			break;
	}
//        gettimeofday(&ut.tx_end_time);

	/* calulating the time elapsed .The below steps is to prevent negative usec values. */

	if(ut.tx_end_time.tv_usec < ut.tx_start_time.tv_usec) {
		int nsec = ( ut.tx_start_time.tv_usec - ut.tx_end_time.tv_usec) / 1000000 + 1;
		ut.tx_start_time.tv_usec -= 1000000 * nsec;
	        ut.tx_start_time.tv_sec += nsec;
	}
	if ((ut.tx_end_time.tv_usec - ut.tx_start_time.tv_usec ) > 1000000) {
		 int nsec = ( ut.tx_end_time.tv_usec - ut.tx_start_time.tv_usec ) / 1000000;
	         ut.tx_start_time.tv_usec += 1000000 * nsec;
		 ut.tx_start_time.tv_usec -= nsec;
	}

	/* Computed the time tv_usec now it is certainly positive value. */
        ut.tx_sec  = ut.tx_end_time.tv_sec  - ut.tx_start_time.tv_sec;
        ut.tx_usec = ut.tx_end_time.tv_usec - ut.tx_start_time.tv_usec;
	/* During Transmission we made the receiver to wait for 5 seconds to receive the break sequence so subtract it */
	if(tx_rx == 'r')
		 ut.tx_sec -= 5;
        printf("\n Time taken %08ld sec, %08ld usec\n\n ",ut.tx_sec,ut.tx_usec);
        /* restore th old port settings. */
        tcsetattr(ut.fd, TCSANOW, &oldtio);
        close(ut.fd);
	exit(0);
}
