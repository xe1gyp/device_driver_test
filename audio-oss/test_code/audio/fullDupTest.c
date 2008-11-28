/* 
 * fullDup.c
 * 
 * This program plays to the audio device with variable options
 * While parallely recording to an output file
 * This requires a RAW input file. the output is also a RAW file
 *
 * Copyright (C) 2004 Texas Instruments, Inc. 
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <linux/soundcard.h>
#include <unistd.h>
#include <string.h>
#include "common.h"
#include "timemeasure.h"

#include <signal.h>

/* Defns */
/* Argument options */
#define ARG_INPUT_FILE   "-i"
#define ARG_OUTPUT_FILE  "-o"
#define ARG_DEVICE       "-d"
#define ARG_VOLUME       "-v"
#define ARG_MIC          "-g"
#define ARG_FREQUENCY    "-f"
#define ARG_BUFFER       "-b"
#define ARG_PLAYBUFFER   "-p"
#define ARG_MONO         "-m"
#define ARG_PERFORMANCE  "-a"
#define ARG_24           "-s"

#define PRINT_ARGS() {int k; printf("ARGS= ");for (k=0;k<argc;k++)printf("%s ",argv[k]);printf ("\n");}
/* Increment the arguments */
#define INC_ARG(ARGUMENT,MAXIMUM)  {(ARGUMENT)++;if (ARGUMENT>MAXIMUM){ USAGE;goto die;}}
#define MY_ERR(ARGS...) fprintf(stderr,"ERROR: " ARGS)
/* Usage */
#define USAGE fprintf (stderr,"Description:- Plays a Raw/PCM File\n"\
	               "Usage:- fulldup %s InputFile  %s OutputFile %s DeviceName %s play_volume %s record_volume %s frequency [%s PlayBufferSize] [%s numberOfPlayBuffers] [%s size] [%s mono_stereo] [%s performance] \n"\
	               "InputFile - The input RAW file with Frequency sampling rate\n"\
	               "OutputFile - The ioutput RAW file To which the recording is to be done to\n"\
		       "DeviceName - /dev/dsp or /dev/sound/dsp the output device name\n"\
		       "play_volume - the volume to play the file - should be in range 0-100\n"\
		       "rec_volume - the volume to record the file - should be in range 0-100\n"\
		       "frequency - the sampling frequency of the input file \n"\
		       "BufferSize - [Optional] the buffer size to use while playing the file\n"\
		       "numberOfBuffers - [Optional] plays only specified number of buffers\n"\
		       "size - [Optional] select 16/24bit support[default 16 bits]\n"\
		       "mono_stereo - [Optional] 1-Mono or 2-stereo[default - stereo]\n"\
		       "performance - [Optional] 0-dont do performance measure, others do[default0]\n",\
		       ARG_INPUT_FILE, ARG_OUTPUT_FILE, ARG_DEVICE, ARG_VOLUME, ARG_MIC, ARG_FREQUENCY, ARG_BUFFER, ARG_PLAYBUFFER, ARG_24, ARG_MONO, ARG_PERFORMANCE)

#define BUFSIZE 8192		/* Default buffer size */
#define RATE 44100		/* the sampling rate */
#define SIZE 16			/* sample size: 8 or 16 bits */
#define CHANNELS 2		/* 1 = mono 2 = stereo */
#define READ_MAX 8192

#define MYSIGNAL SIGUSR1
#define MYSIGNAL1 SIGUSR2

/* GLOBALS */
int audio_dev;
int mixer;
FILE *in_file;
int out_file = 0;

char *audio_dev_name = NULL;	/* Audio Device Name */
char *in_file_name = NULL;	/* Input file Name */
char *out_file_name = NULL;	/* Output file Name */
unsigned char *buffer = NULL;	/* Buffer to use */
char *inter_buffer = NULL;	/* Intermediate buffer to which file will be copied */
int buffer_size = BUFSIZE;	/* number of bytes to be used as buffer */
int file_size = 0;		/* Size of the file */
int num_buffers = -1;

int die_rd = 0;

void ReadDie(int signal_num)
{
	die_rd = 1;
	printf("SIGNALLED\n\n");
}
void reader(void)
{
	int status = 0;		/* Status of the operations  */
	int tot_bytes = 0;
	unsigned char *mybuf[buffer_size];
	unsigned char *my_file_buffer = NULL;

	my_file_buffer = malloc(file_size);

	printf("Reader[%d]\n", buffer_size);
	signal(MYSIGNAL, ReadDie);
	while (!die_rd) {
		status = read(audio_dev, mybuf, buffer_size);
		if (status != buffer_size) {
			printf
			    (" Sorry Pal.. wrong bytes read %d expected, read %d\n",
			     buffer_size, status);
			break;
		}
		/* File operations are costly here.. and they can result 
		 * in buffer overflow and resulting dumps in the driver.
		 * put the data in a temp buffer 
		 * Could have read directly into the final buffer..
		 * but that would not have tested the buffer issues.
		 */
		if ((tot_bytes + buffer_size) > file_size) {
			printf("File Size reached..\n");
			break;
		}
                memcpy(my_file_buffer + tot_bytes, mybuf, buffer_size);
                tot_bytes += buffer_size;
	}
	printf("Reader: Read op complete.. Writing %d bytes to file(thought %d)\n",tot_bytes,file_size);
	status = write(out_file, my_file_buffer, tot_bytes);
	if (status != tot_bytes) {
		printf
		    (" Sorry Pal.. wrong bytes written %d expected, wrote %d\n",
		     tot_bytes, status);
	}
	free(my_file_buffer);
	/* Signal parent that we are done */
	printf("Reader is complete tot %d bytes written\n", tot_bytes);
	kill(getppid(), MYSIGNAL1);
	exit(0);
}

void writer(void)
{
	int tot_read = 0;
	int status = 0;		/* Status of the operations  */
	printf("Writer\n");
	printf("Attempting to play song:\n");
	tot_read = file_size;
	buffer = inter_buffer;
	while (tot_read) {
		int write_size =
		    (tot_read > buffer_size) ? buffer_size : tot_read;
		status = write(audio_dev, buffer, write_size);
		tot_read -= write_size;
		buffer += write_size;
		if (status != write_size) {
			MY_ERR("Could not write");
			USAGE;
			break;
		}
		/* Handle the max buffers */
		if (num_buffers > 0)
			num_buffers--;
		/* if max buffers reached, then quit */
		if (num_buffers == 0)
			break;

	}
	printf("Writer is complete\n");

}

int main(int argc, char **argv)
{
	int status = 0;		/* Status of the operations  */
	int arg = 0;		/* argument for ioctl calls */
	int volume = -1;	/* argument for ioctl calls */
	int mic_volume = -1;	/* argument for ioctl calls */
	int frequency = -1;	/* Frequency of the playback */
	int channels = CHANNELS;
	int size = SIZE;
	int arg_num = 1;	/* counter for the argument */
	struct stat file_stat = { 0 };	/* File stats */
	int child_pid = 0;

	PRINT_ARGS();

	signal(MYSIGNAL1, ReadDie);
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
		/* output file */
		if (0 == strcmp(argument, ARG_OUTPUT_FILE)) {
			INC_ARG(arg_num, argc);
			out_file_name = argv[arg_num];
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
		/* Volume */
		if (0 == strcmp(argument, ARG_VOLUME)) {
			INC_ARG(arg_num, argc);
			sscanf(argv[arg_num], "%d", &volume);
			INC_ARG(arg_num, argc);
			continue;
		}
		/* Mic Volume */
		if (0 == strcmp(argument, ARG_MIC)) {
			INC_ARG(arg_num, argc);
			sscanf(argv[arg_num], "%d", &mic_volume);
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
		/* Size of sample */
		if (0 == strcmp(argument, ARG_24)) {
			INC_ARG(arg_num, argc);
			sscanf(argv[arg_num], "%d", &size);
			INC_ARG(arg_num, argc);
			if (size == 16) {
				size = AFMT_S16_LE;
			} else if ((size == 24) || (size == 32)) {
				size = AFMT_S32_LE;
			} else {
				MY_ERR("unsupported request %d size\n", size);
				exit(1);
			}
			continue;
		}
		/* mono/stereo */
		if (0 == strcmp(argument, ARG_MONO)) {
			INC_ARG(arg_num, argc);
			sscanf(argv[arg_num], "%d", &channels);
			if ((channels < 1) || (channels > 2)) {
				MY_ERR("Bad num channels %d\n", channels);
				exit(1);
			}
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

		/* Max number of buffers */
		if (0 == strcmp(argument, ARG_PLAYBUFFER)) {
			INC_ARG(arg_num, argc);
			sscanf(argv[arg_num], "%d", &num_buffers);
			INC_ARG(arg_num, argc);
			continue;
		}
		MY_ERR("INVALID ARGUMENT: %s\n", argument);
		USAGE;
		goto die;
	}			/* End of while (arg_num<=argc) */

	/* Validate the params */
	if ((0 == buffer_size) || (-1 == frequency) || (-1 == volume)
	    || (-1 == mic_volume)
	    || (NULL == audio_dev_name) || (NULL == in_file_name)
	    || (NULL == out_file_name)) {
		MY_ERR("NOT enuf arguments/invalid arguments\n");
		USAGE;
		exit(2);
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
	inter_buffer = (char *)malloc(file_size);
	if (!inter_buffer) {
		MY_ERR("Failed to allocate %d sized intermediate buffer!\n",
		       file_size);
		USAGE;
		exit(1);
	}

	printf("Input File[%s] size = %d\n", in_file_name, file_size);

	/* open the files */
	in_file = fopen(in_file_name, "rb");
	if (in_file == NULL) {
		MY_ERR("Could not open input file %s\n", in_file_name);
		USAGE;
		goto die;
	}
	/* open the files */
	/* Find the size of the file */
	status = stat(out_file_name, &file_stat);
	if (!status) {
		printf("%s already exists.. deleting oldfile..\n",
		       out_file_name);
		/* File exists.. delete it */
		status = unlink(out_file_name);
		if (status) {
			MY_ERR("Unable to delete file %s\n", out_file_name);
			goto die;
		}
	}
	out_file = open(out_file_name, O_CREAT | O_RDWR);
	if (out_file < 0) {
		MY_ERR("Could not open output file %s\n", out_file_name);
		USAGE;
		goto die;
	}
	/* Buffer the data */
	printf("Buffering Complete Input file: ");
	buffer = inter_buffer;
	while (!feof(in_file)) {
		int read_size = fread(buffer, 1, 1000, in_file);
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
	}
	printf("\bDone\n");

	audio_dev = open(audio_dev_name, O_RDWR);
	if (audio_dev < 0) {
		MY_ERR("Could not open device %s (%x)\n", audio_dev_name,
		       audio_dev);
		USAGE;
		goto die;
	}

	/* set sampling parameters Bit size */
	arg = size;		/* sample size */
	status = ioctl(audio_dev, SOUND_PCM_WRITE_BITS, &arg);
	if (status != 0) {
		MY_ERR("SOUND_PCM_WRITE_BITS ioctl failed");
		goto die;
	}
	if (arg != size) {
		MY_ERR("unable to SOUND_PCM_WRITE_BITS");
		goto die;
	}

	arg = channels;		/* mono or stereo */
	status = ioctl(audio_dev, SOUND_PCM_WRITE_CHANNELS, &arg);
	if (status != 0) {
		MY_ERR("SOUND_PCM_WRITE_CHANNELS ioctl failed");
		goto die;
	}
	if (arg != channels) {
		MY_ERR("unable to set number of channels");
		goto die;
	}

	/* Attempt to read the sample rate */
	status = ioctl(audio_dev, SOUND_PCM_READ_RATE, &arg);
	if (status != 0) {
		MY_ERR("SOUND_PCM_READ_RATE ioctl failed");
		goto die;
	}
	printf("Default Sample Rate = %d\n", arg);
	/* Set the sample rate */
	status = ioctl(audio_dev, SOUND_PCM_WRITE_RATE, &frequency);
	if (status != 0) {
		MY_ERR("SOUND_PCM_WRITE_WRITE ioctl failed");
		goto die;
	}
	/* Attempt to read the sample rate */
	status = ioctl(audio_dev, SOUND_PCM_READ_RATE, &arg);
	if (status != 0) {
		MY_ERR("SOUND_PCM_READ_RATE ioctl failed");
		goto die;
	}
	printf("Read back after setting the Sample Rate = %d\n", arg);

	/* Attempt to read the Bit Size */
	status = ioctl(audio_dev, SNDCTL_DSP_GETFMTS, &arg);
	if (status != 0) {
		MY_ERR("SNDCTL_DSP_GETFMTS ioctl failed");
		goto die;
	}
	printf("Read back BitSize= %d\n", arg);

	/* Attempt to read the Default num channels */
	status = ioctl(audio_dev, SOUND_PCM_READ_CHANNELS, &arg);
	if (status != 0) {
		MY_ERR("SOUND_PCM_READ_CHANNELS ioctl failed");
		goto die;
	}
	printf("Supported channels= %d\n", arg);

	/* Volume settings */
	arg = volume;
	status = ioctl(audio_dev, SOUND_MIXER_WRITE_VOLUME, &arg);
	if (status != 0) {
		MY_ERR("\nSOUND_MIXER_WRITE_VOLUME control failed");
		goto die;
	}
	arg = mic_volume;
	status = ioctl(audio_dev, SOUND_MIXER_WRITE_MIC, &arg);
	if (status != 0) {
		MY_ERR("\nSOUND_MIXER_WRITE_MIC control failed");
		goto die;
	}

	/* Sync up */
	status = ioctl(audio_dev, SOUND_PCM_SYNC, 0);
	if (status != 0) {
		MY_ERR("SOUND_PCM_SYNC ioctl failed");
		goto die;
	}

	if ((child_pid = fork()) == 0) {
		reader();
	} else {
		writer();
		kill(child_pid, MYSIGNAL);
	}
	/* Wait for child process to signal me back */
	while (!die_rd) {
		sleep(1);
		printf("waiting for child to signal me back\n");
	}
	/* how abt a lil over run? */
	printf("Force over run\n");
	sleep(2);
      die:
	if (inter_buffer) {
		free(inter_buffer);
	}
	if (audio_dev >= 0) {
		close(audio_dev);
	}
	if (out_file) {
		fsync(out_file);
		close(out_file);
	}
	if (in_file) {
		fclose(in_file);
	}
	return 0;
}
