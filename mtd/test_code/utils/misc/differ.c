#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <strings.h>
int main(int argc, char **argv)
{
	int fd = 0;
	int fd1 = 0;
	int count = 0;
	int count1 = 512;
	int ret = 0;
	int c;
	char *buff = 0;
	char *buff1 = 0;
	char *file1;
	char *file2;
	if (argc < 4) {
		printf("not enuf argumnets\n");
		return -1;
	}
	sscanf(argv[1], "%x", &count);
	file1 = argv[2];
	file2 = argv[3];
	fd = open(file1, O_RDONLY);
	if (fd < 0) {
		printf("cannot open %s\n", file1);
		ret = -2;
		goto cleanup;
	}
	fd1 = open(file2, O_RDONLY);
	if (fd1 < 0) {
		printf("cannot open %s\n", file2);
		ret = -2;
		goto cleanup;
	}

	buff = (char *)malloc(count1);
	if (0 == buff) {
		printf("could not allocate memory");
		ret = -3;
		goto cleanup;
	}

	buff1 = (char *)malloc(count1);
	if (0 == buff1) {
		printf("could not allocate memory");
		ret = -3;
		goto cleanup;
	}
	printf ("%d %d\n",count, count1);
	while (count) {
		int readsz = (count > count1) ? count1 : count;
		//printf("Readsz=%d count=%d count1=%d\n",readsz,count,count1);
		//if (readsz != (c = read(fd, buff,readsz))) {
		if (1>(c = read(fd, buff,readsz))) {
			printf
			    ("Could not read enuf data in %s. read only %d instead of %d count=%d",
			     file1, c, readsz,count);
			ret = -4;
			goto cleanup;
		}
		readsz=c;
		if (readsz != (c = read(fd1, buff1, readsz))) {
			printf
			    ("Could not read enuf data in %s. read only %d instead of %d count=%d",
			     file2, c, readsz,count);
			ret = -4;
			goto cleanup;
		}

		count -= readsz;
		while (readsz) {
			readsz--;
			if (buff[readsz] != buff1[readsz]) {
				printf("<BEEP>%s and %s are NOT SAME", file1,
				       file2);
				ret = -6;
				goto cleanup;
			}
		}
	}
	if (0 < read(fd, buff, 1)) {
		printf("read more data in %s", file1);
		ret = -5;
		goto cleanup;
	}
	if (0 < read(fd1, buff1, 1)) {
		printf("read more data in %s", file2);
		ret = -5;
		goto cleanup;
	}
      cleanup:
	if (buff1)
		free(buff1);
	if (buff)
		free(buff);
	if (fd1)
		close(fd1);
	if (fd)
		close(fd);

	if (0 == ret)
		printf("%s and %s are SAME", file1, file2);
	printf("\n");
	return ret;
}
