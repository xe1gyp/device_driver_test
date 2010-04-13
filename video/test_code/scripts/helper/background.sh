#!/bin/sh

BG_COLOR=$1
RESULT=0

DEF_BG_COLOR=`cat /sys/devices/platform/omapdss/manager0/default_color`
OVERLAY=`echo /sys/devices/platform/omapdss/overlay0/enabled`

#Usage: setbgcolor <24 bit RGB value>

	echo 0 > $OVERLAY
	$TESTBIN/setbgcolor $BG_COLOR
	sleep 2
	$TESTBIN/setbgcolor $DEF_BG_COLOR
	sleep 2
	echo 1 > $OVERLAY

RESULT=`command_tracking.sh $RESULT $?`

if [ -z "$STRESS" ]; then
	stress_message.sh
fi

exit $RESULT
