#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <strings.h>
int main( int argc, char ** argv)
{
	int count=0;
	int i=0;
	int st=0;
	int k=0;
	if (argc<3){
		printf("not enuf argumnets %d expected\n", 2);
		return -1;
	}
	sscanf (argv[1],"%x",&count);
	sscanf (argv[2],"%d",&st);
	
	k=st;
	for (i=0;i<count;i++)
	{
		printf ("%c",k++);
		if (k>100) k=st;
	}
	return 0;
}
