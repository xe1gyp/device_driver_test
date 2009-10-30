/* Do not edit this file. It was automatically generated. */

#ifndef HEADER_CPUMeter
#define HEADER_CPUMeter
/*
htop - CPUMeter.h
(C) 2004-2006 Hisham H. Muhammad
Released under the GNU GPL, see the COPYING file
in the source distribution for its full text.
*/

#include "Meter.h"

#include "ProcessList.h"

#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include <math.h>

#include "debug.h"
#include <assert.h>

extern int CPUMeter_attributes[];

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

extern MeterType CPUMeter;

extern MeterType AllCPUsMeter;

#endif
