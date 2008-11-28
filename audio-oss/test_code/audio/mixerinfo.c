/*
 * mixer_info.c
 * Example program to display mixer settings
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/soundcard.h>

/* utility function for printing status */
void yes_no(int condition)
{
	condition ? printf("  yes      ") : printf("  no       ");
}

int main(int argc, char *argv[])
{
	int fd;			/* file descriptor for mixer device */
	int i;			/* loop counter */
	int level;		/* volume setting */
	char *device;		/* name of device to report on */
	int status;		/* status of system calls */
	/* various device settings */
	int recsrc, devmask, recmask, stereodevs, caps;
	/* names of available mixer channels */
	const char *sound_device_names[] = SOUND_DEVICE_LABELS;

	mixer_info info;

	/* get device name from command line or use default */
	if (argc == 2)
		device = argv[1];
	else
		device = "/dev/mixer";

	/* open mixer, read only */
	fd = open(device, O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "%s: unable to open `%s', ", argv[0], device);
		perror("");
		return 1;
	}

	/* mixer information */
	status = ioctl(fd, SOUND_MIXER_INFO, &info);
	if (status == -1)
		perror("SOUND_MIXER_INFO ioctl failed");

	info.id[15] = 0;
	info.name[31] = 0;
	info.fillers[9] = 0;
	printf("Mixer information:\n");
	printf("%s (%s): %d modifications\n\n",
	       info.id, info.name, info.modify_counter);

	/* get all of the information about the mixer */
	status = ioctl(fd, SOUND_MIXER_READ_RECSRC, &recsrc);
	if (status == -1)
		perror("SOUND_MIXER_READ_RECSRC ioctl failed");
	status = ioctl(fd, SOUND_MIXER_READ_DEVMASK, &devmask);
	if (status == -1)
		perror("SOUND_MIXER_READ_DEVMASK ioctl failed");
	status = ioctl(fd, SOUND_MIXER_READ_RECMASK, &recmask);
	if (status == -1)
		perror("SOUND_MIXER_READ_RECMASK ioctl failed");
	status = ioctl(fd, SOUND_MIXER_READ_STEREODEVS, &stereodevs);
	if (status == -1)
		perror("SOUND_MIXER_READ_STEREODEVS ioctl failed");
	status = ioctl(fd, SOUND_MIXER_READ_CAPS, &caps);
	if (status == -1)
		perror("SOUND_MIXER_READ_CAPS ioctl failed");

	/* print results in a table */
	printf("Status of %s:\n\n"
	       "Mixer      Device     Recording  Active     Stereo     Current\n"
	       "Channel    Available  Source     Source     Device     Level\n"
	       "---------  ---------  ---------  --------   ---------  ---------\n",
	       device);

	/* loop over all devices */
	for (i = 0; i < SOUND_MIXER_NRDEVICES; i++) {
		/* print number and name */
		printf("%2d %-7s", i, sound_device_names[i]);
		/* print if available */
		yes_no((1 << i) & devmask);
		/* can it be used as a recording source? */
		yes_no((1 << i) & recmask);
		/* it it an active recording source? */
		yes_no((1 << i) & recsrc);
		/* does it have stereo capability? */
		yes_no((1 << i) & stereodevs);
		/* if available, display current level */
		if ((1 << i) & devmask) {
			/* if stereo, show both levels */
			if ((1 << i) & stereodevs) {
				status = ioctl(fd, MIXER_READ(i), &level);
				if (status == -1)
					perror("SOUND_MIXER_READ ioctl failed");
				printf("  %d%% %d%%", level & 0xff,
				       (level & 0xff00) >> 8);
			} else {	/* only one channel */
				status = ioctl(fd, MIXER_READ(i), &level);
				if (status == -1)
					perror("SOUND_MIXER_READ ioctl failed");
				printf("  %d%%", level & 0xff);
			}
		}
		printf("\n");
	}
	printf("\n");
	/* are recording sources exclusive? */
	printf("Note: Choices for recording source are ");
	if (!(caps & SOUND_CAP_EXCL_INPUT))
		printf("not ");
	printf("exclusive.\n");
	/* close mixer device */
	close(fd);
	return 0;
}
