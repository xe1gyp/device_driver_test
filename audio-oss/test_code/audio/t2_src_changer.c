/* 
 * t2_src_changer -
 * This allows to change the source (rec/play).
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

/*OUTPUT:
=======*/
#define DIR_OUT                         (0x1<<28)
#define OUTPUT_STEREO_HEADSET           1
#define OUTPUT_HANDS_FREE_CLASSD        2
#define OUTPUT_MONO_EARPIECE            4

/*INPUT:
======*/
#define DIR_IN                          (0x1<<30)
#define INPUT_HEADSET_MIC               1
#define ONBOARD_MIC			2

/* Argument options */
#define ARG_RECORD_DEVICE   "-i"
#define ARG_OUTPUT_DEVICE   "-o"
#define ARG_DEVICE          "-d"

#define PRINT_ARGS() {int k; printf("ARGS= ");for (k=0;k<argc;k++)printf("%s ",argv[k]);printf ("\n");}

/* Increment the arguments */
#define INC_ARG(ARGUMENT,MAXIMUM)  {(ARGUMENT)++;if (ARGUMENT>MAXIMUM){ USAGE;goto die;}}
#define MY_ERR(ARGS...) fprintf(stderr,"ERROR: " ARGS)

#define BUFSIZE 8192		/* Default buffer size */

#define RATE 44100		/* the sampling rate */
#define SIZE 16			/* sample size: 8 or 16 bits */
#define CHANNELS 2		/* 1 = mono 2 = stereo */

/* Usage */
#define USAGE fprintf (stderr,"Description:- Plays a Raw/PCM File\n"\
	               "Usage:- %s [%s InputDevice] [%s OutputDevice] %s Device\n"\
	               "Input device - Recording Device\n"\
			"  INPUT_HEADSET_MIC         1\n"\
			"  ONBOARD_MIC               2\n"\
			"\nOutputDevice - playback device \n"\
			"  OUTPUT_STEREO_HEADSET     1\n"\
			"  OUTPUT_HANDS_FREE_CLASSD  2\n"\
			"  OUTPUT_MONO_EARPIECE      4\n"\
		       "\nDevice  - /dev/mixer or /dev/sound/mixer the output device name\n",\
		       argv[0],ARG_RECORD_DEVICE, ARG_OUTPUT_DEVICE,ARG_DEVICE)

struct babashuka {
	int t2_val;
	int linux_val;
	char lnx_name[100];
	char device_name[100];
};
#define NUM_BABAS 5
static struct babashuka baba[NUM_BABAS] = {
	{.t2_val = INPUT_HEADSET_MIC | DIR_IN,
	 .linux_val = SOUND_MASK_LINE,
	 .lnx_name = "SOUND_MASK_LINE",
	 .device_name = "INPUT_HEADSET_MIC"},
	{.t2_val = ONBOARD_MIC | DIR_IN,
	 .linux_val = SOUND_MASK_MIC,
	 .lnx_name = "SOUND_MASK_MIC",
	 .device_name = "ONBOARD_MIC"},
	{.t2_val = OUTPUT_STEREO_HEADSET | DIR_OUT,
	 .linux_val = SOUND_MASK_LINE1,
	 .lnx_name = "SOUND_MASK_LINE1",
	 .device_name = "OUTPUT_STEREO_HEADSET"},
	{.t2_val = OUTPUT_HANDS_FREE_CLASSD | DIR_OUT,
	 .linux_val = SOUND_MASK_SPEAKER,
	 .lnx_name = "SOUND_MASK_SPEAKER",
	 .device_name = "OUTPUT_HANDS_FREE_CLASSD"},
	{.t2_val = OUTPUT_MONO_EARPIECE | DIR_OUT,
	 .linux_val = SOUND_MASK_PHONEOUT,
	 .lnx_name = "SOUND_MASK_PHONEOUT",
	 .device_name = "OUTPUT_MONO_EARPIECE"},
};

/************************ FUNCTIONS *****************************/
static int t2_to_linux(int val)
{
	int ret = 0;
	int i = 0;
	for (; i < NUM_BABAS; i++)
		if ((val & baba[i].t2_val) == baba[i].t2_val)
			ret |= baba[i].linux_val;
	return ret;
}
static int linux_to_t2(int val)
{
	int ret = 0;
	int i = 0;
	for (; i < NUM_BABAS; i++)
		if ((val & baba[i].linux_val) == baba[i].linux_val)
			ret |= baba[i].t2_val;
	return ret;
}
static void print_t2_names(int val)
{
	int i = 0;
	for (; i < NUM_BABAS; i++)
		if ((val & baba[i].t2_val) == baba[i].t2_val)
			printf("%s[%s] ", baba[i].device_name,
			       baba[i].lnx_name);
	return;
}

int main(int argc, char **argv)
{
	int Fd = 0;		/* File descriptor for Audio device */

	char *audio_dev_name = NULL;	/* Audio Device Name */
	unsigned int record_device = 0;
	unsigned int playback_device = 0;
	int arg_num = 1;	/* counter for the argument */
	int temp = 0;
	int status = 0;
	int arg = 0;

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
		/* Volume */
		if (0 == strcmp(argument, ARG_RECORD_DEVICE)) {
			INC_ARG(arg_num, argc);
			sscanf(argv[arg_num], "%d", &temp);
			INC_ARG(arg_num, argc);
			record_device |= temp;
			continue;
		}
		/* Frequency */
		if (0 == strcmp(argument, ARG_OUTPUT_DEVICE)) {
			INC_ARG(arg_num, argc);
			sscanf(argv[arg_num], "%d", &temp);
			INC_ARG(arg_num, argc);
			playback_device |= temp;
			continue;
		}
		MY_ERR("INVALID ARGUMENT: %s\n", argument);
		USAGE;
		goto die;
	}			/* End of while (arg_num<=argc) */

	/* Validate the params */
	if ((NULL == audio_dev_name)) {
		MY_ERR("NOT enuf arguments/invalid arguments\n");
		USAGE;
		exit(2);
	}

	Fd = open(audio_dev_name, O_WRONLY);
	if (Fd < 0) {
		MY_ERR("Could not open device %s\n", audio_dev_name);
		USAGE;
		goto die;
	}
	record_device |= DIR_IN;
	playback_device |= DIR_OUT;

	/* Read previous values */
	status = ioctl(Fd, MIXER_READ(SOUND_MIXER_RECSRC), &arg);
	if (status != 0) {
		MY_ERR("SOUND_MIXER_RECSRC READ ioctl failed");
		goto die;
	}
	printf("Current Sources:\nRecord[0x%04x-%d]:", arg, arg);
	print_t2_names(linux_to_t2(arg));
	status = ioctl(Fd, MIXER_READ(SOUND_MIXER_OUTSRC), &arg);
	if (status != 0) {
		MY_ERR("SOUND_MIXER_OUTSRC READ ioctl failed");
		goto die;
	}
	printf("\nPlayback[0x%04x-%d]: ", arg, arg);
	print_t2_names(linux_to_t2(arg));
	printf("\n\nToggling:\nRecord:");
	print_t2_names(record_device);
	printf("\nPlayback: ");
	print_t2_names(playback_device);
	printf("\n");
	if (record_device & ~DIR_IN) {
		arg = t2_to_linux(record_device);
		status = ioctl(Fd, MIXER_WRITE(SOUND_MIXER_RECSRC), &arg);
		if (status != 0) {
			MY_ERR("SOUND_MIXER_RECSRC WRITE ioctl failed");
			goto die;
		}
		/* Read New values */
		status = ioctl(Fd, MIXER_READ(SOUND_MIXER_RECSRC), &arg);
		if (status != 0) {
			MY_ERR("SOUND_MIXER_RECSRC READ ioctl failed");
			goto die;
		}
		printf("New Sources:Record[0x%04x-%d]:", arg, arg);
		print_t2_names(linux_to_t2(arg));
	}
	if (playback_device & ~DIR_OUT) {
		arg = t2_to_linux(playback_device);
		status = ioctl(Fd, MIXER_WRITE(SOUND_MIXER_OUTSRC), &arg);
		if (status != 0) {
			MY_ERR("SOUND_MIXER_OUTSRC WRITE ioctl failed");
			goto die;
		}
		status = ioctl(Fd, MIXER_READ(SOUND_MIXER_OUTSRC), &arg);
		if (status != 0) {
			MY_ERR("SOUND_MIXER_OUTSRC READ ioctl failed");
			goto die;
		}
		printf("\nNew Sources: Playback[0x%04x-%d]: ", arg, arg);
		print_t2_names(linux_to_t2(arg));
	}
	printf("\n");
	/* Exit strategy */
      die:
	if (status) {
		printf("ERR %d[0x%x]\n", status, status);
	}
	if (Fd >= 0) {
		close(Fd);
	}

	return 0;
}
