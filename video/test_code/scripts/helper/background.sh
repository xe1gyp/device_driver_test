#!/bin/sh

VIDEO_PIPELINE=$1
BG_COLOR=$2
RESULT=0

#Usage: setbgcolor <video device> <24 bit RGB value>
$TESTBIN/setbgcolor $VIDEO_PIPELINE $BG_COLOR

RESULT=`command_tracking.sh $RESULT $?`

if [ -z "$STRESS" ]; then
	stress_message.sh
fi

exit $RESULT
