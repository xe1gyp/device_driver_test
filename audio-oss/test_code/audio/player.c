/* 
 * player.c
 * 
 * This program plays to the audio device with variable options
 * This requires a RAW input file.
 *
 * Copyright (C) 2006 Texas Instruments, Inc. 
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
#include "common.h"
#include "timemeasure.h"

/************************ MACROS *****************************/

/* Argument options */
#define ARG_INPUT_FILE   "-i"
#define ARG_DEVICE       "-d"
#define ARG_VOLUME       "-v"
#define ARG_FREQUENCY    "-f"
#define ARG_BUFFER       "-b"
#define ARG_PLAYBUFFER   "-p"
#define ARG_24           "-s"
#define ARG_MONO         "-m"
#define ARG_PERFORMANCE  "-a"

#define PRINT_ARGS() {int k; printf("ARGS= ");for (k=0;k<argc;k++)printf("%s ",argv[k]);printf ("\n");}

/* Increment the arguments */
#define INC_ARG(ARGUMENT,MAXIMUM)  {(ARGUMENT)++;if (ARGUMENT>MAXIMUM){ USAGE;goto die;}}
#define MY_ERR(ARGS...) fprintf(stderr,"ERROR: " ARGS)

#define BUFSIZE 8192		/* Default buffer size */

#define RATE 44100		/* the sampling rate */
#define SIZE AFMT_S16_LE	/* sample size: 8 or 16 bits */
#define CHANNELS 2		/* 1 = mono 2 = stereo */

/* Usage */
#define USAGE fprintf (stderr,"Description:- Plays a Raw/PCM File\n"\
	               "Usage:- player %s InputFile %s DeviceName %s volume %s frequency [%s BufferSize] [%s numberOfBuffers] [%s size] [%s mono_stereo] [%s performance]\n"\
	               "InputFile - The input RAW file with Frequency sampling rate\n"\
		       "DeviceName - /dev/dsp or /dev/sound/dsp the output device name\n"\
		       "volume - the volume to play the file - should be in range 0-100\n"\
		       "frequency - the sampling frequency of the file \n"\
		       "BufferSize - [Optional] the buffer size to use while playing the file\n"\
		       "numberOfBuffers - [Optional] plays only specified number of buffers\n"\
		       "size - [Optional] plays with 24 bit support[default 16 bits]\n"\
		       "mono_stereo - [Optional] 1-Mono or 2-stereo[default - stereo]\n"\
		       "performance - [Optional] 0-dont do performance measure, others do[default0]\n",\
		       ARG_INPUT_FILE, ARG_DEVICE, ARG_VOLUME, ARG_FREQUENCY, ARG_BUFFER, ARG_PLAYBUFFER, ARG_24, ARG_MONO,ARG_PERFORMANCE)

/************************ FUNCTIONS *****************************/

int main(int argc, char **argv)
{
	FILE *Fp = NULL;	/* File pointer to Input File */
	int Fd = 0;		/* File descriptor for Audio device */
	int status = 0;		/* Status of the operations  */
	int buffer_size = BUFSIZE;	/* number of bytes to be used as buffer */
	unsigned char *buffer = NULL;	/* Buffer to use */
	int arg = 0;		/* argument for ioctl calls */
	int volume = -1;	/* argument for ioctl calls */
	int frequency = -1;	/* Frequency of the playback */
	int arg_num = 1;	/* counter for the argument */
	char *audio_dev_name = NULL;	/* Audio Device Name */
	char *in_file_name = NULL;	/* Audio Device Name */
	int num_buffers = -1;
	char *inter_buffer = NULL;	/* Intermediate buffer to which file will be copied */
	int file_size = 0;	/* Size of the file */
	struct stat file_stat = { 0 };	/* File stats */
	int tot_read = 0;
	int size = SIZE;
	int channels = CHANNELS;
	int perf = 0;
	time_measure_t m_open = 0;
	time_measure_t m_close = 0;
	time_measure_t m_write = 0;

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
		/* Volume */
		if (0 == strcmp(argument, ARG_VOLUME)) {
			INC_ARG(arg_num, argc);
			sscanf(argv[arg_num], "%d", &volume);
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

		/* Max number of buffers */
		if (0 == strcmp(argument, ARG_PLAYBUFFER)) {
			INC_ARG(arg_num, argc);
			sscanf(argv[arg_num], "%d", &num_buffers);
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
			if ((channels < 0) || (channels > 2)) {
				MY_ERR("unsupported channels %d\n", channels);
				exit(1);
			}
			INC_ARG(arg_num, argc);
			continue;
		}
		/* performance */
		if (0 == strcmp(argument, ARG_PERFORMANCE)) {
			INC_ARG(arg_num, argc);
			sscanf(argv[arg_num], "%d", &perf);
			INC_ARG(arg_num, argc);
			continue;
		}
		MY_ERR("INVALID ARGUMENT: %s\n", argument);
		USAGE;
		goto die;
	}			/* End of while (arg_num<=argc) */

	/* Validate the params */
	if ((0 == buffer_size) || (-1 == frequency) || (-1 == volume)
	    || (NULL == audio_dev_name) || (NULL == in_file_name)) {
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
#ifdef GSAMPLE_SHIFT
	inter_buffer = (char *)malloc(file_size * 2);
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
	if (Fp == NULL) {
		MY_ERR("Could not open input file %s\n", in_file_name);
		USAGE;
		goto die;
	}
	/* Buffer the data */
	printf("Buffering Complete file: ");
	buffer = inter_buffer;
	while (!feof(Fp)) {
#ifdef GSAMPLE_SHIFT
		unsigned char myBuffer1[1000] = { 0 };
		unsigned char *myBuffer = myBuffer1;
		int read_size = fread(myBuffer, 1, 1000, Fp);
		static int x = 0;
		int myi = 0;

		if (read_size < 0) {
			MY_ERR("Could not read %s- filesize=%d, read %d\n",
			       in_file_name, file_size, read_size);
			USAGE;
			goto die;
		}
		for (myi = 0; myi < read_size; myi += 2) {
#if 1
			unsigned short mahData =
			    *((unsigned short *)(myBuffer));
			unsigned short writeData1 = 0;
			unsigned short writeData2 = 0;
			writeData2 = (mahData & 0xF000) >> 12;
			writeData1 = (mahData & 0x0FFF) << 4;
			*((unsigned short *)(buffer)) = writeData2;
			buffer += 2;
			*((unsigned short *)(buffer)) = writeData1;
#else
			unsigned short temp1 = 0, temp2 = 0;
			unsigned int temp;
			temp = *(unsigned short *)(myBuffer);
			temp <<= 4;

			temp1 = temp >> 16;
			temp2 = temp & 0xffff;
			*((unsigned short *)buffer) = temp1;
			buffer += 2;
			*((unsigned short *)buffer) = temp2;
#endif
			buffer += 2;

			myBuffer += 2;
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

	if (perf) {
		m_open = lbp_user_timereg("open", 4);
		m_close = lbp_user_timereg("close", 5);
		m_write = lbp_user_timereg("write", 5);
	}

	if (perf)
		lbp_user_timestart(m_open);
	Fd = open(audio_dev_name, O_WRONLY);
	if (perf)
		lbp_user_timeend(m_open);

	if (Fd < 0) {
		MY_ERR("Could not open device %s\n", audio_dev_name);
		USAGE;
		goto die;
	}

	/* set sampling parameters Bit size */
	arg = size;		/* sample size */
	status = ioctl(Fd, SOUND_PCM_WRITE_BITS, &arg);
	if (status != 0) {
		MY_ERR("SOUND_PCM_WRITE_BITS ioctl failed");
		goto die;
	}
	if (arg != size) {
		MY_ERR("unable to SOUND_PCM_WRITE_BITS");
		goto die;
	}
	printf("Set 0x%08x", size);
	/* Check bits set */
	status = ioctl(Fd, SOUND_PCM_READ_BITS, &arg);
	if (status != 0) {
		MY_ERR("SOUND_PCM_READ_BITS ioctl failed");
		goto die;
	}
	printf(" Read 0x%08x\n", arg);
	if (arg != size) {
		MY_ERR("unable to SOUND_PCM_READ_BITS");
		goto die;
	}

	arg = channels;		/* mono or stereo */
	status = ioctl(Fd, SOUND_PCM_WRITE_CHANNELS, &arg);
	if (status != 0) {
		MY_ERR("SOUND_PCM_WRITE_CHANNELS ioctl failed");
		goto die;
	}
	if (arg != channels) {
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

	/* Attempt to read the supported Bit Size */
	status = ioctl(Fd, SNDCTL_DSP_GETFMTS, &arg);
	if (status != 0) {
		MY_ERR("SNDCTL_DSP_GETFMTS ioctl failed");
		goto die;
	}
	printf("Read back Fmt= 0x%x\n", arg);

	/* Attempt to read the Default num channels */
	status = ioctl(Fd, SOUND_PCM_READ_CHANNELS, &arg);
	if (status != 0) {
		MY_ERR("SOUND_PCM_READ_CHANNELS ioctl failed");
		goto die;
	}
	printf("Supported channels= %d\n", arg);

	/* Volume settings */
	arg = volume;
	printf("Setting volume:");
	status = ioctl(Fd, SOUND_MIXER_WRITE_VOLUME, &arg);
	if (status != 0) {
		MY_ERR("\nSOUND_MIXER_WRITE_VOLUME control failed");
		goto die;
	}
	printf("passed\n");

	/* Sync up */
	status = ioctl(Fd, SOUND_PCM_SYNC, 0);
	if (status != 0) {
		MY_ERR("SOUND_PCM_SYNC ioctl failed");
		goto die;
	}

	printf("Attempting to play song:\n");
#ifdef GSAMPLE_SHIFT
	tot_read = file_size * 2;
#else
	tot_read = file_size;
#endif
	buffer = inter_buffer;
	while (tot_read) {
		int write_size =
		    (tot_read > buffer_size) ? buffer_size : tot_read;
		static int xyz = 0;
		//printf("%dm-0x%08x\n",xyz,* ( (unsigned int *) (buffer)));
		xyz++;
		if (perf)
			lbp_user_timestart(m_write);

		status = write(Fd, buffer, write_size);
		if (perf)
			lbp_user_timeend(m_write);

		tot_read -= write_size;
		buffer += write_size;
		if (status != write_size) {
			MY_ERR("Could not write");
			USAGE;
			goto die;
		}
		/* Handle the max buffers */
		if (num_buffers > 0)
			num_buffers--;
		/* if max buffers reached, then quit */
		if (num_buffers == 0)
			break;
	}
	printf("Play complete..\n syncing up.. \n");
	/* Sync to complete the playback use the other ioctl ;) */
	status = ioctl(Fd, SNDCTL_DSP_SYNC, 0);
	if (status != 0) {
		MY_ERR("SOUND_PCM_SYNC ioctl failed");
		goto die;
	}

	printf("Sync Complete..\n Force underrun(sleep 2)\n");
	/* Underrun force */
	sleep(2);

	/* Exit strategy */
      die:
	if (inter_buffer) {
		free(inter_buffer);
	}
	if (Fd >= 0) {
		if (perf)
			lbp_user_timestart(m_close);
		close(Fd);
		if (perf)
			lbp_user_timeend(m_close);

	}
	if (Fp) {
		fclose(Fp);
	}
	printf("\n%s Completed \n", argv[0]);
	if (perf) {
		lbp_user_timestats_header();
		lbp_user_timestats(m_open);
		lbp_user_timestats(m_write);
		lbp_user_timestats(m_close);
		lbp_user_timeunreg(m_open);
		lbp_user_timeunreg(m_close);
		lbp_user_timeunreg(m_write);
		printf ("NOTE: Write measurements are for %d bytes each\n",buffer_size);
	}

	return 0;
}
