 /* =======================================================================
 * test_mute.c
 *  Program to test individual PCM stream mute/unmute settings
 *   
 *             Texas Instruments OMAP(TM) Platform Software
 *  Copyright (C) 2007 Texas Instruments, Incorporated.  All Rights Reserved.
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *  
 * Based on amixer.c
 * Copyright (c) 1999-2000 by Jaroslav Kysela <perex@suse.cz>
 * ======================================================================== */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>
#include <errno.h>
#include <assert.h>
#include <sys/poll.h>

#include <alsa/asoundlib.h>
#include <alsa/pcm.h>

#define MUTE_CTL_BASE	36

/* Prototypes */
static int ProcessArgs(int argc, char **argv, int *stream_id,
			int *mute, int *ctl_base);

/* Gets control type */
static const char *control_type(snd_ctl_elem_info_t *info)
{
	return snd_ctl_elem_type_name(snd_ctl_elem_info_get_type(info));
}

/* Gets control access info */
static const char *control_access(snd_ctl_elem_info_t *info)
{
	static char result[10];
	char *res = result;

	*res++ = snd_ctl_elem_info_is_readable(info) ? 'r' : '-';
	*res++ = snd_ctl_elem_info_is_writable(info) ? 'w' : '-';
	*res++ = snd_ctl_elem_info_is_inactive(info) ? 'i' : '-';
	*res++ = snd_ctl_elem_info_is_volatile(info) ? 'v' : '-';
	*res++ = snd_ctl_elem_info_is_locked(info) ? 'l' : '-';
	*res++ = snd_ctl_elem_info_is_tlv_readable(info) ? 'R' : '-';
	*res++ = snd_ctl_elem_info_is_tlv_writable(info) ? 'W' : '-';
	*res++ = snd_ctl_elem_info_is_tlv_commandable(info) ? 'C' : '-';
	*res++ = '\0';
	return result;
}

/* 
 * This shows the control parameters
 */
static int show_control(const char *space, snd_hctl_elem_t *elem)
{
	int err;
	static char card[64] = "default";
	unsigned int item, idx, count;
	snd_ctl_elem_type_t type;
	snd_ctl_elem_id_t *id;
	snd_ctl_elem_info_t *info;
	snd_ctl_elem_value_t *control;
	snd_ctl_elem_id_alloca(&id);
	snd_ctl_elem_info_alloca(&info);
	snd_ctl_elem_value_alloca(&control);
	if ((err = snd_hctl_elem_info(elem, info)) < 0) {
		fprintf(stderr, "Control %s snd_hctl_elem_info error: %s\n",
			card, snd_strerror(err));
		return err;
	}

	count = snd_ctl_elem_info_get_count(info);
	type = snd_ctl_elem_info_get_type(info);
	printf("%s; type=%s,access=%s,values=%i",
		space, control_type(info), control_access(info), count);
	switch (type) {
	case SND_CTL_ELEM_TYPE_INTEGER:
		printf(",min=%li,max=%li,step=%li\n", 
		       snd_ctl_elem_info_get_min(info),
		       snd_ctl_elem_info_get_max(info),
		       snd_ctl_elem_info_get_step(info));
		break;
	case SND_CTL_ELEM_TYPE_INTEGER64:
		printf(",min=%Li,max=%Li,step=%Li\n", 
		       snd_ctl_elem_info_get_min64(info),
		       snd_ctl_elem_info_get_max64(info),
		       snd_ctl_elem_info_get_step64(info));
		break;
	case SND_CTL_ELEM_TYPE_ENUMERATED:
	{
		unsigned int items = snd_ctl_elem_info_get_items(info);
		printf(",items=%u\n", items);
		for (item = 0; item < items; item++) {
			snd_ctl_elem_info_set_item(info, item);
			if ((err = snd_hctl_elem_info(elem, info)) < 0) {
				fprintf(stderr,
					"Control %s element info error: %s\n",
					card, snd_strerror(err));
				return err;
			}
			printf("%s; Item #%u '%s'\n", space, item,
				snd_ctl_elem_info_get_item_name(info));
		}
		break;
	}
	default:
		printf("\n");
		break;
	}

	if ((err = snd_hctl_elem_read(elem, control)) < 0) {
		fprintf(stderr, "Control %s element read error: %s\n",
			card, snd_strerror(err));
		return err;
	}
	printf("%s: values=", space);
	for (idx = 0; idx < count; idx++) {
		if (idx > 0)
			printf(",");
		switch (type) {
		case SND_CTL_ELEM_TYPE_BOOLEAN:
			printf("%s",
				snd_ctl_elem_value_get_boolean(control, idx) ?
				"on" : "off");
			break;
		case SND_CTL_ELEM_TYPE_INTEGER:
			printf("%li",
				snd_ctl_elem_value_get_integer(control, idx));
			break;
		case SND_CTL_ELEM_TYPE_INTEGER64:
			printf("%Li",
				snd_ctl_elem_value_get_integer64(control, idx));
			break;
		case SND_CTL_ELEM_TYPE_ENUMERATED:
			printf("%u",
				snd_ctl_elem_value_get_enumerated(control,
				idx));
		break;
		case SND_CTL_ELEM_TYPE_BYTES:
			printf("0x%02x",
				snd_ctl_elem_value_get_byte(control, idx));
			break;
		default:
			printf("?");
			break;
		}
	}
	printf("\n");

	return 0;
}

/*
 *  To get what type of interface the control(id) belongs to
 */ 
static const char *control_iface(snd_ctl_elem_id_t *id)
{
	return snd_ctl_elem_iface_name(snd_ctl_elem_id_get_interface(id));
}

/*
 * Shows the control information, but not its parameters
 */ 
static void show_control_id(snd_ctl_elem_id_t *id)
{
	unsigned int index, device, subdevice;
	printf("numid=%u,iface=%s,name='%s'",
	       snd_ctl_elem_id_get_numid(id),
	       control_iface(id),
	       snd_ctl_elem_id_get_name(id));
	index = snd_ctl_elem_id_get_index(id);
	device = snd_ctl_elem_id_get_device(id);
	subdevice = snd_ctl_elem_id_get_subdevice(id);
	if (index)
		printf(",index=%i", index);
	if (device)
		printf(",device=%i", device);
	if (subdevice)
		printf(",subdevice=%i", subdevice);
}

/*
 * Store arguments from user
 */ 
static int ProcessArgs(int argc, char **argv, int *stream_id,
			int *mute, int *ctl_base)
{
	int status = -1;

	if (argc < 3) {
		fprintf(stdout, "Usage: %s <stream_id> <mute> \n",argv[0]);
		fprintf(stdout, "Or: %s <stream_id> <mute> <ctl_base> ",
			argv[0]);
		fprintf(stdout, " Default ctl_base value is: %d \n",
			MUTE_CTL_BASE);
	} else if (argc == 3) {
		*stream_id = atoi(argv[1]);
		*mute = atoi(argv[2]);
		status = 0;
	} else if (argc == 4) {
		*stream_id = atoi(argv[1]);
		*mute = atoi(argv[2]);
		*ctl_base = atoi(argv[3]);
		status = 0;
	}
	return status;
}

int main (int argc, char **argv ){

	int err;
	int status;
	static char card[64] = "default";
	static int quiet = 0;
	int stream_id = 0;
	int mute = 0;
	int ctl_base = 0;
	static snd_ctl_t *handle = NULL;
	snd_ctl_elem_info_t *info;
	snd_ctl_elem_id_t *id;
	snd_ctl_elem_value_t *control;
	unsigned int count, idx;
	unsigned int ctl_num = MUTE_CTL_BASE;
	/* This is the default base id of the control to modify.
	 * Mute/unmute controls start at MUTE_CTL_BASE.
	 * There are 16 controls registered for 16 different streams.
	 * To control a particular stream, it adds the base number + stream_id
	 * as offset to access the specific control for that stream.
	 * This base control id can be modified by the user entering a new
	 * base id control when runnig this test.
	 */
	long tmp;
	snd_ctl_elem_type_t type;

	status = ProcessArgs(argc, argv, &stream_id, &mute, &ctl_base);
	if (status)
		return status;
	if (ctl_base)
		ctl_num = (unsigned int)ctl_base;
	fprintf(stdout,
		"Requesting Mute/Unmute of 0x%x for stream 0x%x \n", 
		mute, stream_id);
	
	ctl_num += stream_id;

	snd_ctl_elem_info_alloca(&info);
	snd_ctl_elem_id_alloca(&id);
	snd_ctl_elem_value_alloca(&control);
	
	snd_ctl_elem_id_set_numid(id, ctl_num);
	
	snd_ctl_elem_id_set_interface(id, SND_CTL_ELEM_IFACE_PCM);

	fprintf(stdout,"setting the mute/unmute for stream index 0x%x \n", 
		(unsigned int) stream_id);

	if (handle == NULL &&
	    (err = snd_ctl_open(&handle, card, 0)) < 0) {
		fprintf(stderr,"Control %s open error: %s\n", 
			card, snd_strerror(err));
		return err;
	}

	snd_ctl_elem_info_set_id(info, id);
	if ((err = snd_ctl_elem_info(handle, info)) < 0) {
		fprintf(stderr, 
			"Cannot find the given element from control %s\n",
			card);
		return err;
	}

	snd_ctl_elem_info_get_id(info, id);
	type = snd_ctl_elem_info_get_type(info);
	count = snd_ctl_elem_info_get_count(info);
	snd_ctl_elem_value_set_id(control, id);

	show_control_id(id);
	printf("\n");
	
	/* These are the APIs that modify the specific control value,
	   depending on the type of control: boolean, integer, enumerated
	   idx is for controls that have more than one value to modify, 
	   for example stereo controls (count = 2)
	 */
	for (idx = 0; idx < count; idx++) {
		switch (type) {
		case SND_CTL_ELEM_TYPE_BOOLEAN:
			tmp = 0;
			snd_ctl_elem_value_set_boolean(control, idx, mute);
			break;
		case SND_CTL_ELEM_TYPE_INTEGER:
			tmp = 50;
			snd_ctl_elem_value_set_integer(control, idx, tmp);
			break;
		case SND_CTL_ELEM_TYPE_INTEGER64:
			tmp = 0;
			snd_ctl_elem_value_set_integer64(control, idx, tmp);
			break;
		case SND_CTL_ELEM_TYPE_ENUMERATED:
			tmp = 0;
			snd_ctl_elem_value_set_enumerated(control, idx, tmp);
			break;
		case SND_CTL_ELEM_TYPE_BYTES:
			tmp = 0;
			snd_ctl_elem_value_set_byte(control, idx, tmp);
			break;
		default:
			break;
		}
	}
       /* This changes the value set previously. This API is necessary to
          actually change the value of the control.
`	*/
	if ((err = snd_ctl_elem_write(handle, control)) < 0) {
		fprintf(stderr, "Control %s element write error: %s\n", 
			card, snd_strerror(err));
		return err;
	}

	snd_ctl_close(handle);

	/* This part displays the actual values of the controls */
	if (!quiet) {
		snd_hctl_t *hctl;
		snd_hctl_elem_t *elem;
		if ((err = snd_hctl_open(&hctl, card, 0)) < 0) {
			fprintf(stderr, "Control %s open error: %s\n", 
				card, snd_strerror(err));
			return err;
		}
		if ((err = snd_hctl_load(hctl)) < 0) {
			fprintf(stderr, "Control %s load error: %s\n", 
				card, snd_strerror(err));
			return err;
		}
		elem = snd_hctl_find_elem(hctl, id);
		if (elem)
			show_control("  ", elem);
		else
			printf("Could not find the specified element\n");
		snd_hctl_close(hctl);
	}

	return 0;
}
