#!/bin/sh

VIDEO_PIPELINE=$1
SETIMG_PARAMETERS=$2
SETWIN_PARAMETERS=$3
RESULT=0

# Usage: setimg <vid> <fmt> <width> <height>
$TESTBIN/setimg $VIDEO_PIPELINE $SETIMG_PARAMETERS

if [ ! -z "$SETWIN_PARAMETERS" ]; then
	# Usage: setwin <vid> <left> <top> <width> <height>
	$TESTBIN/setwin $VIDEO_PIPELINE $SETWIN_PARAMETERS
	RESULT=`command_tracking.sh $RESULT $?`
fi
RESULT=`command_tracking.sh $RESULT $?`

exit $RESULT
