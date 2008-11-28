/* 
 * recorder.c
 * 
 * This program records a predefined length of seconds of data from 
 * the audio device to a predefined file
 * Note: this will generate a RAW file output
 *
 * Copyright (C) 2006 Texas Instruments, Inc. 
 */

/************************ INCLUDES *****************************/

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <linux/soundcard.h>
#include <unistd.h>
#include <string.h>
#include "common.h"
#include "timemeasure.h"

/************************ MACROS *****************************/

#define PRINT_ARGS() {int k; printf("ARGS= ");for (k=0;k<argc;k++)printf("%s ",argv[k]);printf ("\n");}
#define MY_ERR(ARGS...) fprintf(stderr,"ERROR: " ARGS)
#define USAGE fprintf (stderr,"Description:- Records a Raw/PCM File \n"\
	               "Usage:- recorder %s DeviceName [%s volume] [%s frequency] [%s size] [%s mono_stereo] [%s duration] [%s outputfile] [%s underrun] [%s performance] [%s buffer_size\n"\
		       "DeviceName - /dev/dsp or /dev/sound/dsp the output device name\n"\
		       "volume - the volume to play the file - should be in range 0-100\n"\
		       "frequency - the sampling frequency of the file [default 44100] \n"\
		       "size - [Optional] select 16/24bit support[default 16 bits]\n"\
		       "duration - [Optional] Duration of recording in seconds [default=2]\n"\
		       "outputfile - [Optional] File to dump to [default=./test.raw]\n"\
		       "underrun - [Optional] Force a underrun[default=no underrun -0]\n"\
		       "mono_stereo - [Optional] 1-Mono or 2-stereo[default - stereo]\n"\
		       "performance - [Optional] 0-dont do performance measure, others do[default0]\n"\
		       "buffer_size - [Optional] Read from dsp and write to file in chunks of .. [default file_size]\n",\
		       ARG_DEVICE, ARG_VOLUME, ARG_FREQUENCY, ARG_24, ARG_MONO, ARG_TIME, ARG_FILE, ARG_UNDERRUN,ARG_PERFORMANCE,ARG_BUFFER)

#define INC_ARG(ARGUMENT,MAXIMUM)  {(ARGUMENT)++;if (ARGUMENT>MAXIMUM){ USAGE;exit (1);}}

#define LENGTH 5		/* no. of seconds of speech to dump */
#define RATE 44100		/* the sampling rate */
#define SIZE AFMT_S16_LE	/* sample size: 8 or 16 bits */
#define CHANNELS 2		/* 1 = mono 2 = stereo */
#define ARG_VOLUME       "-v"
#define ARG_FREQUENCY    "-f"
#define ARG_24           "-s"
#define ARG_MONO         "-m"
#define ARG_DEVICE       "-d"
#define ARG_TIME         "-t"
#define ARG_FILE         "-o"
#define ARG_UNDERRUN     "-u"
#define ARG_PERFORMANCE  "-a"
#define ARG_BUFFER       "-b"

/************************ FUNCTIONS *****************************/
int main(int argc, char **argv)
{

	int fd = 0;
	struct stat file_stat = { 0 };	/* File stats */
	int status = 0;
	int fd2 = 0;
	int arg = 0;
	int arg_num = 1;
	int volume = -1;
	int rate = RATE;
	int channels = CHANNELS;
	int size = SIZE;
	char *audio_dev_name = NULL;	/* Audio Device Name */
	char *default_file_name = "./test.raw";
	char *output_file_name = default_file_name;	/* Audio Device Name */
	int alloc_size = -1;
	int completed_size = 0;
	int file_size = 0;
	char *buffer = NULL;
	char *file_buffer = NULL;
	int time = 2;
	int underrun = 0;
	int perf = 0;
	time_measure_t m_open = -1;
	time_measure_t m_close = -1;
	time_measure_t m_read = -1;
	time_measure_t m_tot = -1;
	time_measure_t m_fswr = -1;

	PRINT_ARGS();

	/* Capture the parameters */
	while (arg_num < argc) {
		char *argument = argv[arg_num];
		/* audio device */
		if (0 == strcmp(argument, ARG_DEVICE)) {
			INC_ARG(arg_num, argc);
			audio_dev_name = argv[arg_num];
			INC_ARG(arg_num, argc);
			continue;
		}
		/* output file */
		if (0 == strcmp(argument, ARG_FILE)) {
			INC_ARG(arg_num, argc);
			output_file_name = argv[arg_num];
			INC_ARG(arg_num, argc);
			continue;
		}
		/* Volume */
		if (0 == strcmp(argument, ARG_BUFFER)) {
			INC_ARG(arg_num, argc);
			sscanf(argv[arg_num], "%d", &alloc_size);
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
		/* Sample rate */
		if (0 == strcmp(argument, ARG_FREQUENCY)) {
			INC_ARG(arg_num, argc);
			sscanf(argv[arg_num], "%d", &rate);
			INC_ARG(arg_num, argc);
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
		/* Duration */
		if (0 == strcmp(argument, ARG_TIME)) {
			INC_ARG(arg_num, argc);
			sscanf(argv[arg_num], "%d", &time);
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
		INC_ARG(arg_num, argc);
	}
	/* bytes to be allocated =
	 * time in seconds
	 * samples per second -> frequency
	 * number of bytes per sample -> 2/4
	 * number of channels -> mono=1/stereo=2
	 */
	file_size = time * rate * ((size == AFMT_S16_LE) ? 2 : 4) * channels;
	if ((alloc_size == -1) || (alloc_size > file_size)) {
		alloc_size = file_size;
	}
	printf("Sample rate = %8d hertz\n", rate);
	printf("Duration    = %8d seconds\n", time);
	printf("Bit Size    = %8d bytes\n", ((size == AFMT_S16_LE) ? 2 : 4));
	printf("Mono/Stereo = %8s\n", ((channels == 1) ? "Mono" : "Stereo"));
	printf("Buffer Size = %8d bytes\n", alloc_size);
	printf("File Size   = %8d bytes\n", file_size);
	printf("Device File = %8s\n", audio_dev_name);
	printf("Output File = %8s\n", output_file_name);
	if (volume != -1) {
		printf("Volume Level= Left = %2d Right = %2d\n", volume & 0xFF,
		       (volume >> 8) & 0xFF);
	}

	if (!audio_dev_name || !output_file_name) {
		printf("File Name Missing device = %s o/p file=%s!!\n",
		       audio_dev_name, output_file_name);
		goto bad;
	}
	buffer = (char *)malloc(alloc_size);
	if (!buffer) {
		printf("Could not allocate %d memory!!\n", alloc_size);
		goto bad;
	}
	if (file_size > alloc_size) {
		file_buffer = (char *)malloc(file_size + alloc_size);
	} else {
		file_buffer = (char *)malloc(file_size);
	}
	if (!file_buffer) {
		printf("Could not allocate %d memory!!\n", alloc_size);
		goto bad;
	}
	if (perf) {
		m_open = lbp_user_timereg("open", 4);
		m_close = lbp_user_timereg("close", 5);
		m_read = lbp_user_timereg("read", 4);
		m_tot = lbp_user_timereg("Total_read", 4);
		m_fswr = lbp_user_timereg("fswrite", 4);
	}
	if (perf)
		lbp_user_timestart(m_open);
	fd = open(audio_dev_name, O_RDONLY);
	if (perf)
		lbp_user_timeend(m_open);
	if (fd < 0) {
		printf("open of [%s] failed\n", audio_dev_name);
		goto bad;
	}

	arg = size;
	status = ioctl(fd, SOUND_PCM_WRITE_BITS, &arg);
	if (status) {
		printf("SOUND_PCM_WRITE_BITS ioctl failed\n");
		goto bad;
	}
	if (arg != size) {
		printf("Verify of samplesize failed\n");
		goto bad;
	}

	arg = channels;
	status = ioctl(fd, SOUND_PCM_WRITE_CHANNELS, &arg);
	printf(" - - - - - %d", channels);
	if (status) {
		printf("SOUND_PCM_WRITE_CHANNELS ioctl failed\n");
		goto bad;
	}
	if (arg != channels) {
		printf("Unable to set number of channels\n");
		goto bad;
	}

	arg = rate;
	status = ioctl(fd, SOUND_PCM_WRITE_RATE, &arg);

	if (status) {
		printf("SOUND_PCM_WRITE_RATE ioctl failed\n");
		goto bad;
	}

	/* Volume settings */
	if (volume > -1) {
		arg = volume;
		status = ioctl(fd, SOUND_MIXER_WRITE_MIC, &arg);
		if (status != 0) {
			MY_ERR("\nSOUND_MIXER_MIC_VOLUME control failed");
			goto bad;
		}
	}
	/* Find the size of the file */
	status = stat(output_file_name, &file_stat);
	if (!status) {
		printf("%s already exists.. deleting oldfile..\n",
		       output_file_name);
		/* File exists.. delete it */
		status = unlink(output_file_name);
		if (status) {
			MY_ERR("Unable to delete file %s\n", output_file_name);
			goto bad;
		}
	}

	fd2 = open(output_file_name, O_CREAT | O_WRONLY);
	if (fd2 < 0) {
		printf("open of file %s failed\n", output_file_name);
		goto bad;
	}
	printf("Recording Now:\n");
	if (perf)
		lbp_user_timestart(m_tot);
	completed_size = 0;
	while (completed_size < file_size) {
		if (perf)
			lbp_user_timestart(m_read);
		status = read(fd, buffer, alloc_size);
		if (perf)
			lbp_user_timeend(m_read);
		if (status != alloc_size) {
			printf
			    ("Read wrong number of bytes - %d instead of %d\n",
			     status, alloc_size);
			goto bad;
		}
		if ((completed_size + alloc_size) > file_size) {
			printf("completed=%d, alloc=%d, file_size=%d\n",
			       completed_size, alloc_size, file_size);
		}
		/* File operations are costly here.. and they can result 
		 * in buffer overflow and resulting dumps in the driver.
		 * put the data in a temp buffer */
		memcpy(file_buffer + completed_size, buffer, alloc_size);
		completed_size += alloc_size;
	}
	if (perf)
		lbp_user_timeend(m_tot);

	if (perf)
		lbp_user_timestart(m_fswr);
	status = write(fd2, file_buffer, completed_size);
	if (perf)
		lbp_user_timeend(m_fswr);

	if (status != completed_size) {
		printf
		    ("Wrote wrong number of bytes to %s %d instead of %d\n",
		     output_file_name, status, completed_size);
		goto bad;
	}
	printf("Thank you. Recording is complete\n");

	if (underrun) {
		printf("Force under-run sleep for %d seconds: ", underrun);
		sleep(underrun);
		printf("Complete\n");
	}

	if (perf)
		lbp_user_timestart(m_close);
	close(fd);
	if (perf)
		lbp_user_timeend(m_close);
	/* Flush the write buffer to the storage */
	fsync(fd2);
	close(fd2);

	if (file_buffer) {
		free(file_buffer);
	}
	if (buffer) {
		free(buffer);
	}
	if (perf) {
		lbp_user_timestats_header();
		lbp_user_timestats(m_open);
		lbp_user_timestats(m_read);
		lbp_user_timestats(m_tot);
		lbp_user_timestats(m_close);
		lbp_user_timestats(m_fswr);
		lbp_user_timeunreg(m_open);
		lbp_user_timeunreg(m_close);
		lbp_user_timeunreg(m_read);
		lbp_user_timeunreg(m_tot);
		lbp_user_timeunreg(m_fswr);
		printf("NOTE: Read measurements are for %d bytes\n",
		       alloc_size);
	}

	return 0;
      bad:
	printf("ERROR Happend!! Quitting!!\n");
	if (m_open >= 0) {
		lbp_user_timeunreg(m_open);
	}
	if (m_close >= 0) {
		lbp_user_timeunreg(m_close);
	}
	if (m_tot >= 0) {
		lbp_user_timeunreg(m_tot);
	}
	if (m_read >= 0) {
		lbp_user_timeunreg(m_read);
	}
	if (buffer) {
		free(buffer);
	}
	if (fd2 >= 0) {
		close(fd2);
	}
	if (fd >= 0) {
		close(fd);
	}
	USAGE;
	return 1;
}
