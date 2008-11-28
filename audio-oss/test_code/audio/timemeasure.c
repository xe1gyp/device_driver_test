/* 
 * timemeasure.c
 * 
 * Library to measure time interevals b/w function calls
 *
 * Copyright (C) 2005 Texas Instruments, Inc. 
 */

/* includes */
#include "timemeasure.h"
#include <sys/time.h>
#include <string.h>
#include <stdio.h>

/* MODULE DEPENDENT MACROS */
#define NOTUSED 0
#define USED 1
#define error(ARGS...) fprintf (stderr,ARGS)
#define mesg(ARGS...) fprintf (stdout,ARGS)

/* MODULE DEPENDENT Data structures */
struct __measurement_point {
	char name[MAX_NAME_SIZE];
	unsigned char used;
	unsigned char undermeasure;
	unsigned int numberofevents;
	struct timeval totaltimevalue;
	struct timeval currenttimevalue;
	struct timezone timezone;

};

struct __measurement_point measure[MAX_MEASUREMENT_POINTS] = { 0 };

/*************** GLOBAL FUNCTIONS */

/* register a measurement point 
 * - will return a unique id
 */
time_measure_t lbp_user_timereg(char *name, int length)
{
	int count = 0, free = -1;
	/* Validate params */
	if ((NULL == name) || (length > MAX_NAME_SIZE)) {
		error("Invalid call");
		return -1;
	}
	/* Search for pre-existing point */
	while (count < MAX_MEASUREMENT_POINTS) {
		if ((USED == measure[count].used)
		    && (0 == strncmp(measure[count].name, name, length))) {
			error("pre-existing measurement point %s", name);
			return -1;
		}
		/* Find the first empty place */
		else if ((-1 == free) && (NOTUSED == measure[count].used))
			free = count;
		count++;
	}

	/* if we have got the place to put this up */
	if (-1 != free) {
		/* reset values */
		memset(&(measure[free]), 0, sizeof(struct __measurement_point));
		measure[free].used = USED;
		memcpy(&(measure[free].name), name, length);
	}
	/* return id */
	return (time_measure_t) free;
}

/* unregister a measurement point */
int lbp_user_timeunreg(time_measure_t id)
{
#ifdef STRICT_CHECK
	if ((id < 0) || (id > MAX_MEASUREMENT_POINTS)
	    || (measure[(int)id].used != USED)) {
		error("attempt to release invalid stuff");
		return -1;
	}
#endif
	measure[(int)id].used = NOTUSED;
	return 0;
}

/* notify the start of measurement point */
int lbp_user_timestart(time_measure_t id)
{
	int ret = 0;
	int myid = (int)id;
	/* no checks.. no point in wasting time here -enable strict checks if u want to do the checks */
#ifdef STRICT_CHECK
	if ((myid < 0) || (myid > MAX_MEASUREMENT_POINTS)
	    || (measure[myid].used != USED)) {
		error("attempt to start with invalid id");
		return -1;
	}
#endif
	ret =
	    gettimeofday(&(measure[myid].currenttimevalue),
			 &((measure[myid].timezone)));
	return ret;

}

/* notify the end of measurement point */
int lbp_user_timeend(time_measure_t id)
{
	struct timeval newtimevalue = { 0 };
	struct timezone timezone = { 0 };
	long lTimeDelta;

	int ret = 0;
	int myid = (int)id;
#ifdef STRICT_CHECK
	if ((myid < 0) || (myid > MAX_MEASUREMENT_POINTS)
	    || (measure[myid].used != USED)) {
		error("attempt to start with invalid id");
		return -1;
	}
#endif
	ret = gettimeofday(&newtimevalue, &timezone);
	if (0 == ret) {
		lTimeDelta =
		    (newtimevalue.tv_sec -
		     measure[myid].currenttimevalue.tv_sec) * 1000000 +
		    (newtimevalue.tv_usec -
		     measure[myid].currenttimevalue.tv_usec);

		measure[myid].totaltimevalue.tv_sec += lTimeDelta / 1000000;
		measure[myid].totaltimevalue.tv_usec += lTimeDelta % 1000000;
		measure[myid].numberofevents++;
	}
	return ret;
}

int lbp_user_timestats_header()
{
	/* prints message in this format:*/
	mesg("%8s : %8s %8s %8s\n", "Name",
	     "NumEvs", "Avg Sec", "Avg uSec");
	mesg("-------- : -------- -------- --------\n");
}
/* print stats for the measurement point */
int lbp_user_timestats(time_measure_t id)
{
	long avgsec = 0;
	long avgusec = 0;
	int myid = (int)id;
	long lTimeDelta;
#ifdef STRICT_CHECK
	if ((myid < 0) || (myid > MAX_MEASUREMENT_POINTS)
	    || (measure[myid].used != USED)) {
		error("attempt to start with invalid id");
		return -1;
	}
#endif
	if (measure[myid].numberofevents > 0) {
		lTimeDelta =
		     measure[myid].totaltimevalue.tv_sec * 1000000 +
		     measure[myid].totaltimevalue.tv_usec;
		lTimeDelta /= measure[myid].numberofevents;
		avgsec =lTimeDelta / 1000000;
		avgusec =  lTimeDelta % 1000000; 
	}

	/* prints message in this format:*/
	mesg("%8s : %8d %8d %8d\n", measure[myid].name,
	     measure[myid].numberofevents, avgsec, avgusec);
	return 0;
}

/************ MODULE FUNCTIONS */
