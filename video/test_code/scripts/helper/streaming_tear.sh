#!/bin/sh

VIDEO_PIPELINE=$1
SETIMG_PARAMETERS=$2
SETCROP_PARAMETERS=$3
SETWIN_PARAMETERS=$4
PATTERN1=$5
PATTERN2=$6

RESULT=0

# Usage: setimg <vid> <fmt> <width> <height>
$TESTBIN/setimg $VIDEO_PIPELINE $SETIMG_PARAMETERS

if [ ! -z "$SETCROP_PARAMETERS" ]; then
	# Usage: setcrop <vid> <left> <top> <width> <height>
	$TESTBIN/setcrop $VIDEO_PIPELINE $SETCROP_PARAMETERS
	RESULT=`command_tracking.sh $RESULT $?`
fi

if [ ! -z "$SETWIN_PARAMETERS" ]; then
	# Usage: setwin <vid> <left> <top> <width> <height>
	$TESTBIN/setwin $VIDEO_PIPELINE $SETWIN_PARAMETERS
	RESULT=`command_tracking.sh $RESULT $?`
fi


# Usage: streaming_tear </dev/video_device> <pattern1> <pattern2> [<n>]
$TESTBIN/streaming_tear /dev/video$VIDEO_PIPELINE $PATTERN1 $PATTERN2
RESULT=`command_tracking.sh $RESULT $?`

if [ -z "$STRESS" ]; then
	stress_message.sh
fi

exit $RESULT
