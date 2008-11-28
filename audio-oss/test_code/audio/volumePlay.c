/* 
 * volumePlay.c
 * 
 * This program plays to the audio device with variable options
 * This requires a RAW input file.
 * Further, this attempts to increase and the decrease the volume 
 * after every buffer write
 *
 * Copyright (C) 2004 Texas Instruments, Inc. 
 */

/************************ INCLUDES *****************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/soundcard.h>
#include <unistd.h>
#include <string.h>

/************************ MACROS *****************************/

/* Argument options */
#define ARG_INPUT_FILE   "-i"
#define ARG_DEVICE       "-d"
#define ARG_INCREMENT    "-I"
#define ARG_FREQUENCY    "-f"
#define ARG_BUFFER       "-b"

#define PRINT_ARGS() {int k; printf("ARGS= ");for (k=0;k<argc;k++)printf("%s ",argv[k]);printf ("\n");}

/* Increment the arguments */
#define INC_ARG(ARGUMENT,MAXIMUM)  {(ARGUMENT)++;if (ARGUMENT>MAXIMUM){ USAGE;goto die;}}
#define MY_ERR(ARGS...) fprintf(stderr,"ERROR: " ARGS)

#define BUFSIZE 8192		/* Default buffer size */

#define RATE 44100		/* the sampling rate */
#define SIZE 16			/* sample size: 8 or 16 bits */
#define CHANNELS 2		/* 1 = mono 2 = stereo */

#define VOL_MIN 0
#define VOL_MAX 100
/* Usage */
#define USAGE fprintf (stderr,"Description:- Plays a Raw/PCM File by raising and lowering the volume\n"\
	               "Usage:- player %s InputFile %s DeviceName %s Increment %s frequency [%s BufferSize]\n"\
	               "InputFile - The input RAW file with Frequency sampling rate\n"\
		       "DeviceName - /dev/dsp or /dev/sound/dsp the output device name\n"\
		       "Increment - the volume increase step size use for e.g. -1 to start off\n"\
		       "frequency - the sampling frequency of the file \n"\
		       "BufferSize - [Optional] the buffer size to use while playing the file\n",\
		       ARG_INPUT_FILE, ARG_DEVICE,ARG_INCREMENT, ARG_FREQUENCY, ARG_BUFFER)

/************************ FUNCTIONS *****************************/

int main(int argc, char **argv)
{
	FILE *Fp = NULL;	/* File pointer to Input File */
	int Fd = 0;		/* File descriptor for Audio device */
	int status = 0;		/* Status of the operations  */
	int buffer_size = BUFSIZE;	/* number of bytes to be used as buffer */
	unsigned char *buffer = NULL;	/* Buffer to use */
	int arg = 0;		/* argument for ioctl calls */
	int volume = 0;		/* argument for ioctl calls */
	int inc = 0;		/* Volume increment step size */
	int frequency = -1;	/* Frequency of the playback */
	int arg_num = 1;	/* counter for the argument */
	char *audio_dev_name = NULL;	/* Audio Device Name */
	char *in_file_name = NULL;	/* Audio Device Name */
	char *inter_buffer = NULL;	/* Intermediate buffer to which file will be copied */
	int file_size = 0;	/* Size of the file */
	struct stat file_stat = { 0 };	/* File stats */
	int tot_read = 0;

	PRINT_ARGS();
	/* Capture the parameters */
	while (arg_num < argc) {
		char *argument = argv[arg_num];
		/* input file */
		if (0 == strcmp(argument, ARG_INPUT_FILE)) {
			INC_ARG(arg_num, argc);
			in_file_name = argv[arg_num];
			INC_ARG(arg_num, argc);
			continue;
		}
		/* audio device */
		if (0 == strcmp(argument, ARG_DEVICE)) {
			INC_ARG(arg_num, argc);
			audio_dev_name = argv[arg_num];
			INC_ARG(arg_num, argc);
			continue;
		}
		/* Increment */
		if (0 == strcmp(argument, ARG_INCREMENT)) {
			INC_ARG(arg_num, argc);
			sscanf(argv[arg_num], "%d", &inc);
			INC_ARG(arg_num, argc);
			continue;
		}
		/* Frequency */
		if (0 == strcmp(argument, ARG_FREQUENCY)) {
			INC_ARG(arg_num, argc);
			sscanf(argv[arg_num], "%d", &frequency);
			INC_ARG(arg_num, argc);
			continue;
		}
		/* Buffer Size */
		if (0 == strcmp(argument, ARG_BUFFER)) {
			INC_ARG(arg_num, argc);
			sscanf(argv[arg_num], "%d", &buffer_size);
			INC_ARG(arg_num, argc);
			continue;
		}
		MY_ERR("INVALID ARGUMENT: %s\n", argument);
		USAGE;
		goto die;
	}			/* End of while (arg_num<=argc) */

	/* Validate the params */
	if ((0 == buffer_size) || (-1 == frequency) || (NULL == audio_dev_name)
	    || (NULL == in_file_name) || (0 == inc)) {
		MY_ERR("NOT enuf arguments/invalid arguments\n");
		USAGE;
		exit(2);
	}

	/* Hack - too small sized data, forget testing this- test with player.c */
	if (4 >= buffer_size) {
		return 0;
	}

	/* Find the size of the file */
	status = stat(in_file_name, &file_stat);
	if (status) {
		MY_ERR("Stat of %s returned failure=%d\n", in_file_name,
		       status);
		USAGE;
		exit(1);
	}
	file_size = file_stat.st_size;
#ifdef GSAMPLE_SHIFT
	inter_buffer = (char *)malloc(file_size*2);
#else
	inter_buffer = (char *)malloc(file_size);
#endif
	if (!inter_buffer) {
		MY_ERR("Failed to allocate %d sized intermediate buffer!\n",
		       file_size);
		USAGE;
		exit(1);
	}
	printf("Input File[%s] size = %d\n", in_file_name, file_size);

	/* open the files */
	Fp = fopen(in_file_name, "rb");
	if ((Fp == NULL) || (Fd < 0)) {
		MY_ERR("Could not open input file %s\n", in_file_name);
		USAGE;
		exit(1);
	}

	/* Buffer the data */
	printf("Buffering Complete file: ");
	buffer = inter_buffer;
	while (!feof(Fp)) {
#ifdef GSAMPLE_SHIFT
		unsigned char myBuffer[1000]={0};
		int read_size = fread(myBuffer, 1, 1000, Fp);
		static int x = 0;
		int myi=0;

		if (read_size < 0) {
			MY_ERR("Could not read %s- filesize=%d, read %d\n",
			       in_file_name, file_size, read_size);
			USAGE;
			goto die;
		}
		//printf ("read %d\n",read_size);
		for (myi=0; myi<read_size; myi+=2) {
			unsigned short mahData=*((unsigned short *) (myBuffer+myi));
			unsigned int writeData=0;
			writeData= mahData<<4;
			*((unsigned int *)(buffer))=writeData;
		//	printf ("%d - 0x%04x - 0x%08x - 0x%08x(0x%p)\n",myi,mahData,writeData, *((unsigned int *)(buffer)),buffer);
			buffer+=4;
		}
		if (x < 0) {
			printf("\b \b");
			x--;
			if (x < -11)
				x = 0;
		} else {
			printf(".");
			x++;
			if (x > 10)
				x = -1;
		}
#else
		int read_size = fread(buffer, 1, 1000, Fp);
		static int x = 0;
		if (read_size < 0) {
			MY_ERR("Could not read %s- filesize=%d, read %d\n",
			       in_file_name, file_size, read_size);
			USAGE;
			goto die;
		}
		buffer += read_size;
		if (x < 0) {
			printf("\b \b");
			x--;
			if (x < -11)
				x = 0;
		} else {
			printf(".");
			x++;
			if (x > 10)
				x = -1;
		}
#endif
	}
	printf("\bDone\n");

	Fd = open(audio_dev_name, O_WRONLY);
	if ((Fp == NULL) || (Fd < 0)) {
		MY_ERR("Could not open device %s\n", audio_dev_name);
		USAGE;
		goto die;
	}

	/* set sampling parameters Bit size */
	arg = SIZE;		/* sample size */
	status = ioctl(Fd, SOUND_PCM_WRITE_BITS, &arg);
	if (status != 0) {
		MY_ERR("SOUND_PCM_WRITE_BITS ioctl failed");
		goto die;
	}
	if (arg != SIZE) {
		MY_ERR("unable to SOUND_PCM_WRITE_BITS");
		goto die;
	}

	arg = CHANNELS;		/* mono or stereo */
	status = ioctl(Fd, SOUND_PCM_WRITE_CHANNELS, &arg);
	if (status != 0) {
		MY_ERR("SOUND_PCM_WRITE_CHANNELS ioctl failed");
		goto die;
	}
	if (arg != CHANNELS) {
		MY_ERR("unable to set number of channels");
		goto die;
	}

	/* Attempt to read the sample rate */
	status = ioctl(Fd, SOUND_PCM_READ_RATE, &arg);
	if (status != 0) {
		MY_ERR("SOUND_PCM_READ_RATE ioctl failed");
		goto die;
	}
	printf("Default Sample Rate = %d\n", arg);
	/* Set the sample rate */
	status = ioctl(Fd, SOUND_PCM_WRITE_RATE, &frequency);
	if (status != 0) {
		MY_ERR("SOUND_PCM_WRITE_WRITE ioctl failed");
		goto die;
	}
	/* Attempt to read the sample rate */
	status = ioctl(Fd, SOUND_PCM_READ_RATE, &arg);
	if (status != 0) {
		MY_ERR("SOUND_PCM_READ_RATE ioctl failed");
		goto die;
	}
	printf("Read back after setting the Sample Rate = %d\n", arg);

	/* Attempt to read the Bit Size */
	status = ioctl(Fd, SNDCTL_DSP_GETFMTS, &arg);
	if (status != 0) {
		MY_ERR("SNDCTL_DSP_GETFMTS ioctl failed");
		goto die;
	}
	printf("Read back BitSize= %d\n", arg);

	/* Attempt to read the Default num channels */
	status = ioctl(Fd, SOUND_PCM_READ_CHANNELS, &arg);
	if (status != 0) {
		MY_ERR("SOUND_PCM_READ_CHANNELS ioctl failed");
		goto die;
	}
	printf("Supported channels= %d\n", arg);

	/* Volume settings */
	arg = volume;
	status = ioctl(Fd, SOUND_MIXER_WRITE_VOLUME, &arg);
	if (status != 0) {
		MY_ERR("\nSOUND_MIXER_WRITE_VOLUME control failed");
		goto die;
	}

	/* Sync up */
	status = ioctl(Fd, SOUND_PCM_SYNC, 0);
	if (status != 0) {
		MY_ERR("SOUND_PCM_SYNC ioctl failed");
		goto die;
	}

	printf("Attempting to play song:\n");
	tot_read = file_size;
	buffer = inter_buffer;
	while (tot_read) {
		int write_size =
		    (tot_read > buffer_size) ? buffer_size : tot_read;
		status = write(Fd, buffer, write_size);
		tot_read -= write_size;
		buffer += write_size;
		if (status != write_size) {
			MY_ERR("Could not write");
			USAGE;
			goto die;
		}
		arg = volume;
		status = ioctl(Fd, SOUND_MIXER_WRITE_VOLUME, &arg);
		if (status != 0) {
			MY_ERR("\nSOUND_MIXER_WRITE_VOLUME control failed");
			goto die;

		}
		volume += inc;
		if ((volume > VOL_MAX) || (volume < VOL_MIN))
			inc *= -1;
		if (volume < VOL_MIN)
			volume = VOL_MIN;
		if (volume > VOL_MAX)
			volume = VOL_MAX;

	}
	printf("\n%s Completed \n", argv[0]);

	/* Exit strategy */
      die:
	if (inter_buffer) {
		free(inter_buffer);
	}
	if (Fd >= 0) {
		close(Fd);
	}
	if (Fp) {
		fclose(Fp);
	}

	return 0;
}
