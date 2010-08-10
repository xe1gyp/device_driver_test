#include <stdio.h>

#define ARGB32 1
#define F801 2

static int usage(void)
{
	printf("Usage: createRGB <output_file> <format>\n\
	format :\n\t 1. ARGB32\n\t 2. RGB565 - f801\n");

	return 1;
}

int main(int argc, char *argv[])
{
	FILE *fp;
	int i, j, k, format;

	if (argc != 3)
		return usage();

	fp = fopen(argv[1], "wb");

	if (!fp) {
		printf("Could not open output file %s\n", argv[1]);
		return 1;
	}

	format = atoi(argv[2]);

	if (format == ARGB32) {
		char c1[4] = {255, 0, 0, 250};
		char c2[4] = {255, 0 , 0, 25};

		for (k = 0; k < 10; k++) {
			c2[3] = (k+1)*25;
			for (j = 0; j < 6; j++) {
				for (i = 0; i < 800*40; i++)
					fwrite(c1, sizeof(c1[0]),
						sizeof(c1)/sizeof(c1[0]), fp);
				for (i = 0; i < 800*40; i++)
					fwrite(c2, sizeof(c2[0]),
						sizeof(c2)/sizeof(c2[0]), fp);
			}
		}
	} else {
		short int c1 = 63489, c2 = 31;
		for (k = 0; k < 5; k++) {
			for (j = 0; j < 6; j++) {
				for (i = 0; i < 800*40; i++)
					fwrite(&c1, sizeof(c1), 1, fp);

				for (i = 0; i < 800*40; i++)
					fwrite(&c2, sizeof(c2), 1, fp);
			}
		}
	}

	fclose(fp);
	return 0;
}
