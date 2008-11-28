#include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>
  #include <sys/types.h>
       #include <unistd.h>



int main(int argc, char * argv[])
{
	char * fileName;
	char * mono;
	int bits;
	int skip=0;
	int count=100;
	int count1=0;
	int m=-1;
	int fd;
	int ret=0;
	if (argc<4) {
		printf ("Not enough arguments= filename m/s 16/32 [count] [skip]\n");
		return -1;
	}
	fileName=argv[1];
	mono=argv[2];
	sscanf(argv[3],"%d",&bits);
	if (!strcmp(mono,"m")){
		m=1;
	}
	if (!strcmp(mono,"s")){
		m=2;
	}
	if (m==-1){
		printf ("mono[%s]=m/s\n",mono);
		return -1;
	}
	if ((bits != 32) && (bits != 16)) {
		printf ("bits[%d]=16/32\n",bits);
		return -1;
	}
	/* bits become bytes now! */
	bits=bits/8;
	if (argc >= 5) {
		sscanf(argv[4],"%d",&count);
	}
	if (argc >= 6) {
		sscanf(argv[4],"%d",&skip);
	}
	fd=open (fileName,O_RDONLY);
	if (fd <0) {
		printf ("%s not found\n",fileName);
		return -1;
	}
	ret = lseek(fd,skip*bits*m,SEEK_SET);
	if (ret <0) {
		printf ("skip %d(%d bytes) failed\n",skip,skip*bits*m);
		return -1;
	}
	while(count>count1) {
		short a;
		int b;
		int c;
		if (bits==2) {
			ret=read(fd,&a,2);
			c=a;
		}else {
			ret=read(fd,&b,4);
			c=(b>>(32-24));
		}
		if (ret<0) {
			printf ("failed to read in offset count1=%d\n",count1);
			return -1;
		}
		printf ("%d %d\n",count1,c);
		if (m=2) {
			ret=read(fd,&c,bits);
		}
		count1++;
	}
	close(fd);
	return 0;
}
