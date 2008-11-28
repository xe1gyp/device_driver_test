/* 
 * sineGen.c
 * 
 * generates sinewave data file for a given set of options
 * Compile using:  gcc -lm -o sineGen sineGen.c
 *
 * Copyright (C) 2005 Texas Instruments, Inc. 
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define ARG_SAMPLING_FREQUENCY "-f"
#define ARG_DATA_FREQUENCY "-s"
#define ARG_MAX_DATA "-d"
#define ARG_OFILE "-o"
#define ARG_SEC "-t"
#define ARG_QUIET "-v"
#define ARG_MONO "-m"
#define ARG_ZERO "-z"
#define ARG_24 "-24"
#define ARG_SQUARE "-sq"
#define ARG_HEX "-hex"
#define ARG_DEC "-dec"
#define ARG_SIGN  "-sign"

/* ONE RAD = 360/(2* M_PI) */
//#define RAD 57.295780181884765625
//#define RAD 57.2957795
#define RAD 57.295779513082320876798154814114

#define USAGE \
	printf("Usage:\n %s " \
			ARG_OFILE " output_file_name " \
			"[" ARG_QUIET "] " \
			"[" ARG_HEX "] " \
			"[" ARG_DEC "] " \
			"[" ARG_MONO "] " \
			"[" ARG_SIGN "] " \
			"[" ARG_24 "] " \
			"[" ARG_SQUARE "] " \
			"[" ARG_ZERO " which_to_zero] " \
			"[" ARG_MAX_DATA " maximum_data_size] " \
			"[" ARG_SEC " num_sec_of_data] " \
			"[" ARG_DATA_FREQUENCY " sample_rate_of_output] " \
			"[" ARG_SAMPLING_FREQUENCY " codec_sampling_rate] \n",argv[0]);\
	printf("Options:[] are optional\n" \
			ARG_OFILE " - output_file_name\n" \
			ARG_HEX   " - Hexdecimal output\n" \
			ARG_DEC   " - Decimal output\n" \
			ARG_SIGN  " - UnSigned data format[default is signed]\n" \
			ARG_24  " -  24 bit data[default is 16 bit data]\n" \
			ARG_QUIET " - verbose -  dumb messsages pls {default =  quiet}\n" \
			ARG_MONO  " - mono data- 16 bit left only {default = stereo}\n" \
			ARG_ZERO " - if 0, the first chunk is 0ed, else if 1, the second chunk is zeroed {default = -1-no zero} \n" \
			"     NOTE: Dont use with mono option - u get 0z or this is ignored......\n"\
			ARG_SQUARE  " - generate square wave {default = sinewave}\n" \
			ARG_MAX_DATA " - maximum_data_size {default = 0xffff} \n" \
			ARG_SEC " - num_sec_of_data {default = 1}\n" \
			ARG_DATA_FREQUENCY " - sample_rate_of_output  {default = 800}\n" \
			ARG_SAMPLING_FREQUENCY " - codec_sampling_rate {default = 44100}\n");

#define INC_ARG1(ARGUMENT,MAXIMUM)  {(ARGUMENT)++;\
	                            if (ARGUMENT>=MAXIMUM){\
					    MY_ERR("Insuff arguments\n"); \
					    USAGE;\
					    goto die;\
				    }}
#define INC_ARG(ARGUMENT,MAXIMUM)  {(ARGUMENT)++;\
	                            if (ARGUMENT> MAXIMUM){\
					    MY_ERR("Insuff arguments\n"); \
					    USAGE;\
					    goto die;\
				    }}
#define MY_ERR(ARGS...) fprintf(stderr,"ERROR: " ARGS)

#define PRINT_ARGS() {int k; printf("ARGS= ");for (k=0;k<argc;k++)printf("%s ",argv[k]);printf ("\n");}

int main(int argc, char **argv)
{
	double f_codec = 44100;
	double f_sample = 800;
	float max = 0xffff / 2;
	double num_samples = 0;
	double deg_inc = 0;
	double rad_inc = 0;
	double rad_start = 0;
	int count = 0;
	char *ofile = NULL;
	int arg_num = 1;	/* counter for the argument */
	FILE *Fd = NULL;
	long byte_count = 1;
	char quiet = 1;
	char mono = 0;
	int zero = -1;
	int square = 0;
	int width = 2;
	int hex = 0;
	int dec = 0;
	int fmt = 0;
	char fmt_string[10] = { 0 };
	char sign = 1;
	char selfmt[10] = { 0 };

	PRINT_ARGS();

	/* Capture the parameters */
	while (arg_num < argc) {
		char *argument = argv[arg_num];
		if (0 == strcmp(argument, ARG_OFILE)) {
			INC_ARG1(arg_num, argc);
			ofile = argv[arg_num];
			INC_ARG(arg_num, argc);
			continue;
		}
		if (0 == strcmp(argument, ARG_MAX_DATA)) {
			int val;
			INC_ARG1(arg_num, argc);
			sscanf(argv[arg_num], "%x", &val);
			INC_ARG(arg_num, argc);
			max = val / 2;
			continue;
		}
		if (0 == strcmp(argument, ARG_DATA_FREQUENCY)) {
			int val;
			INC_ARG1(arg_num, argc);
			sscanf(argv[arg_num], "%d", &val);
			INC_ARG(arg_num, argc);
			f_sample = val;
			continue;
		}
		if (0 == strcmp(argument, ARG_ZERO)) {
			int val;
			INC_ARG1(arg_num, argc);
			sscanf(argv[arg_num], "%d", &val);
			INC_ARG(arg_num, argc);
			zero = val;
			continue;
		}
		if (0 == strcmp(argument, ARG_SAMPLING_FREQUENCY)) {
			int val;
			INC_ARG1(arg_num, argc);
			sscanf(argv[arg_num], "%d", &val);
			INC_ARG(arg_num, argc);
			f_codec = val;
			continue;
		}
		if (0 == strcmp(argument, ARG_24)) {
			width = 4;
			if ((int)max == (int)0xffff / 2) {
				printf("resetting max\n");
				max = 0xFFFFFF / 2;
			}
			INC_ARG(arg_num, argc);
			continue;
		}
		if (0 == strcmp(argument, ARG_SIGN)) {
			sign = 0;
			INC_ARG(arg_num, argc);
			continue;
		}
		if (0 == strcmp(argument, ARG_MONO)) {
			mono = 1;
			INC_ARG(arg_num, argc);
			continue;
		}
		if (0 == strcmp(argument, ARG_DEC)) {
			dec = 1;
			INC_ARG(arg_num, argc);
			continue;
		}
		if (0 == strcmp(argument, ARG_HEX)) {
			hex = 1;
			INC_ARG(arg_num, argc);
			continue;
		}
		if (0 == strcmp(argument, ARG_QUIET)) {
			quiet = 0;
			INC_ARG(arg_num, argc);
			continue;
		}
		if (0 == strcmp(argument, ARG_SQUARE)) {
			square = 1;
			INC_ARG(arg_num, argc);
			continue;
		}
		if (0 == strcmp(argument, ARG_SEC)) {
			int val;
			INC_ARG1(arg_num, argc);
			sscanf(argv[arg_num], "%d", &val);
			INC_ARG(arg_num, argc);
			byte_count = val;
			continue;
		}
		MY_ERR("INVALID ARGUMENT: %s\n", argument);
		USAGE;
		goto die;
	}			/* End of while (arg_num<=argc) */
	fmt = (dec || hex);
	if (dec) {
		strcpy(selfmt, "%d\n");
	} else if (hex) {
		strcpy(selfmt, "0x%02x\n");
	}
	num_samples = f_codec / f_sample;
#if 0
	deg_inc = 360 / num_samples;
	rad_inc = deg_inc / RAD;
	printf("f_sample=%d Rad=%f\n", f_sample, rad_inc);
#else
	rad_inc = ((float)2 * (float)M_PI * (float)f_sample) / ((float)f_codec);
	printf("f_sample=%d Rad=%f\n", f_sample, rad_inc);
#endif
	byte_count *= f_codec;

	/* open the files */
	Fd = fopen(ofile, "wb");
	if (Fd == NULL) {
		MY_ERR("UNABLE TO USE %s File\n", ofile);
		USAGE;
		goto die;
	}
	rad_start = 0;
	while (byte_count > 0) {
		for (count = 0; count < num_samples; count++) {
			float val = sin(rad_start);
			int cur = (int)(val * ((float)max));
			unsigned int t_cur = 0;
			if (!sign)
				cur += max;
			if (square) {
				if (count <= num_samples / 2) {
					cur = max * 2;
				} else {
					cur = 0;
				}
			}
			if (!quiet)
				printf
				    ("byte_count=%d count= %3d,sine(%8.9f)=%8.9f\t Cur= %04x\n",
				     (int)byte_count, (int)count,
				     rad_start, val, cur);
			rad_start += rad_inc;

#if 0
			/* when to turn around..-> dont want overflow */
			if (rad_start > rad_inc * num_samples * f_codec) {
				rad_start = 0;
			}
#endif
			if (0 == zero) {
				t_cur = 0;
			} else {
				t_cur = cur;
			}
			if (width == 4) {
				/* 8 bits of padding LSB */
				t_cur <<= 8;
			}
			if (!fmt) {
				if (1 != fwrite(&t_cur, width, 1, Fd)) {
					MY_ERR
					    ("Unable to write to %s File\n",
					     ofile);
					USAGE;
					goto die;
				}
			} else {
				int size = 0;
				size = sprintf(fmt_string, selfmt, t_cur);
				if (size != fwrite(fmt_string, 1, size, Fd)) {
					MY_ERR
					    ("Unable to write to %s File\n",
					     ofile);
					USAGE;
					goto die;
				}
			}
			if (1 == zero) {
				t_cur = 0;
			} else {
				t_cur = cur;
			}
			if (width == 4) {
				/* 8 bits of padding LSB */
				t_cur <<= 8;
			}
			/* left and right synchronized.. */
			if (!mono) {
				if (!fmt) {
					if (1 != fwrite(&t_cur, width, 1, Fd)) {
						MY_ERR
						    ("Unable to write to %s File\n",
						     ofile);
						USAGE;
						goto die;
					}
				} else {
					int size = 0;
					size =
					    sprintf(fmt_string, selfmt, t_cur);
					if (size !=
					    fwrite(fmt_string, 1, size, Fd)) {
						MY_ERR
						    ("Unable to write to %s File\n",
						     ofile);
						USAGE;
						goto die;
					}
				}
			}
			byte_count--;
			if (byte_count <= 0)
				break;

		}
	}
	fclose(Fd);
	return 0;
      die:
	return -1;
}
