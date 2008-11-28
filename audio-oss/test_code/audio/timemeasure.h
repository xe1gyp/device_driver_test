/* 
 * timemeasure.h
 * 
 * Header to measure time interevals b/w function calls
 *
 * Copyright (C) 2005 Texas Instruments, Inc. 
 */
#ifndef __LBP_USER_TIME_MEASURE
#define __LBP_USER_TIME_MEASURE

/* Data structure to define the measurement point id*/
typedef int time_measure_t;

/* MAX Measurement points */
#define MAX_MEASUREMENT_POINTS 20
/* MAX name size */
#define MAX_NAME_SIZE 40

/* Do a strict check of params -comment this out while compiling lib for slightly better performance*/
#define STRICT_CHECK

/* register a measurement point 
 * - will return a unique id
 */
time_measure_t lbp_user_timereg(char * name,int length);

/* unregister a measurement point */
int lbp_user_timeunreg(time_measure_t id);

/* notify the start of measurement point */
int lbp_user_timestart( time_measure_t id );

/* notify the end of measurement point */
int lbp_user_timeend( time_measure_t id );

/* print stats for the measurement point 
 * "%s\t:%d\t%d\t%d\n", name, numevents,
 * avgsec,avgusec
 */
int lbp_user_timestats( time_measure_t id );

int lbp_user_timestats_header();

/* USAGE:
 * call the lbp_user_timereg("whateverNameuwant", length of the name)
 * it returns an id. use the same id in all future calls
 * say start and end for every measure.
 * at the end of the measures call stats function
 * All calls return -1 if it fails
 */
#endif
