#!/bin/sh
###################################################################################################################
# Configuration script
# Author  : Misael Lopez Cruz
# Date    : September 19, 2006
# Description: Configuration script with user specified value for some environment variables
###################################################################################################################

#If we have a working rtc, this is good.. else use next line
export POSTFIX=`date "+%Y%m%d-%H%M%S"`

# Load up the defaults
export TESTROOT=${PWD}
export TESTBIN=${PWD}/../bin
export TESTSCRIPT=${PWD}/helper
export TMPBASE=${TESTROOT}/tmp
export TMPFILE=${TMPBASE}/tmp.$POSTFIX
export CMDFILE=cmd.$POSTFIX
export TESTDIR=${TESTROOT}/test
export PRETTY_PRT=""
export VERBOSE=""
export OUTPUTFILE=${TESTROOT}/output.$POSTFIX
export LOGFILE=${TESTROOT}/log.$POSTFIX
export DURATION="1h"
export PATH="${PATH}:${TESTROOT}:${TESTBIN}:${TESTSCRIPT}"
export TC_SCENARIO="${TESTROOT}/scenarios"
export SCENARIO_NAMES=""
export INTERACTIVE=""
export STRESS=""
export UTILBIN=${TESTROOT}/../../utils/bin
export STRESSARG=""
export SYSVID=/sys/class/display_control/omap_disp_control/video1
export SYSCAMPWD=/sys/devices/platform/omap34xxcam.100/power/state
export TESTCASE=""
export SANITY=""
export SANITYARG=""

export MESSAGE_DELAY=1
export WAIT_ANSWER=$UTILBIN/akey
export WAIT_KEY=$UTILBIN/waitkey
export HELP=""
