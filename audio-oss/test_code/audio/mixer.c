/*
 * mixer.c
 * Example of a simple mixer program
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/soundcard.h>

/* names of available mixer devices */
const char *sound_device_names[] = SOUND_DEVICE_NAMES;

int fd;				/* file descriptor for mixer device */
int devmask, stereodevs;	/* bit masks of mixer information */
char *name;			/* program name */

/* display command usage and exit with error status */
void usage()
{
	int i;

	fprintf(stderr, "usage: %s <device> <left-gain%%> <right-gain%%>\n"
		"       %s <device> <gain%%>\n\n"
		"Where <device> is one of:\n", name, name);
	for (i = 0; i < SOUND_MIXER_NRDEVICES; i++)
		if ((1 << i) & devmask)	/* only display valid devices */
			fprintf(stderr, "%s ", sound_device_names[i]);
	fprintf(stderr, "\n");
	exit(1);
}

int main(int argc, char *argv[])
{
	int left, right, level;	/* gain settings */
	int status;		/* return value from system calls */
	int device;		/* which mixer device to set */
	int i;			/* general purpose loop counter */
	char *dev;		/* mixer device name */
	char *node;
	char *mixerdev;

	/* save program name */
	name = argv[0];

	/* open mixer, read only */
	fd = open("/dev/mixer", O_RDONLY);
	if (fd == -1) {
		fd = open("/dev/sound/mixer", O_RDONLY);
		if (fd == -1) {
			perror("unable to open /dev/mixer or /dev/sound/mixer");
			exit(1);
		}
	}

	/* get needed information about the mixer */
	status = ioctl(fd, SOUND_MIXER_READ_DEVMASK, &devmask);
	if (status == -1)
		perror("SOUND_MIXER_READ_DEVMASK ioctl failed");
	status = ioctl(fd, SOUND_MIXER_READ_STEREODEVS, &stereodevs);
	if (status == -1)
		perror("SOUND_MIXER_READ_STEREODEVS ioctl failed");

	/* check that user passed two or three arguments on command line */
	if (argc != 3 && argc != 4)
		usage();

	/* save mixer device name */
	dev = argv[1];

	/* figure out which device to use */
	for (i = 0; i < SOUND_MIXER_NRDEVICES; i++)
		if (((1 << i) & devmask) && !strcmp(dev, sound_device_names[i]))
			break;
	if (i == SOUND_MIXER_NRDEVICES) {	/* didn't find a match */
		fprintf(stderr, "%s is not a valid mixer device\n", dev);
		usage();
	}

	/* we have a valid mixer device */
	device = i;

	/* get gain values */
	if (argc == 4) {
		/* both left and right values given */
		left = atoi(argv[2]);
		right = atoi(argv[3]);
	} else {
		/* left and right are the same */
		left = atoi(argv[2]);
		right = atoi(argv[2]);
	}

	/* display warning if left and right gains given for non-stereo device */
	if ((left != right) && !((1 << i) & stereodevs)) {
		fprintf(stderr, "warning: %s is not a stereo device\n", dev);
	}

	/* encode both channels into one value */
	level = (right << 8) + left;

	/* set gain */
	fprintf(stderr, "setting gain left=%d right=%d level=%d\n",left,right,level);
	status = ioctl(fd, MIXER_WRITE(device), &level);
	if (status == -1) {
		perror("MIXER_WRITE ioctl failed");
		exit(1);
	}

	fprintf(stderr, "Just check for crash\n");
	/* unpack left and right levels returned by sound driver */
	left = level & 0xff;
	right = (level & 0xff00) >> 8;

	/* display actual gain setting */
	fprintf(stderr, "%s gain set to %d%% / %d%%\n", dev, left, right);

	/* close mixer device and exit */
	close(fd);
	return 0;
}
