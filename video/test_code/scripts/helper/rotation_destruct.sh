#!/bin/sh

VIDEO_PIPELINE=$1
SETIMG_PARAMETERS=$2
STREAMING_PARAMETERS=$3
SETWIN_PARAMETERS=$4
ANGLE=$5
TIME_PARAMETER=$6
RESULT=0

# Usage: setimg <vid> <fmt> <width> <height>
$TESTBIN/setimg $VIDEO_PIPELINE $SETIMG_PARAMETERS

if [ ! -z "$SETWIN_PARAMETERS" ]; then
	# Usage: setwin <vid> <left> <top> <width> <height>
	$TESTBIN/setwin $VIDEO_PIPELINE $SETWIN_PARAMETERS
	RESULT=`command_tracking.sh $RESULT $?`
fi

# Usage: setrotation <vid> <angle>
$TESTBIN/setrotation $VIDEO_PIPELINE $ANGLE
RESULT=`command_tracking.sh $RESULT $?`

# Usage: streaming <vid> <inputfile> [<n>]
if [ ! -z "$TIME_PARAMETER" ]; then
	$TESTBIN/streaming $VIDEO_PIPELINE $STREAMING_PARAMETERS $TIME_PARAMETER
	RESULT=`command_tracking.sh $RESULT $?`
else
	$TESTBIN/streaming $VIDEO_PIPELINE $STREAMING_PARAMETERS 0
	RESULT=`command_tracking.sh $RESULT $?`
fi

$TESTBIN/setrotation $VIDEO_PIPELINE 0
RESULT=`command_tracking.sh $RESULT $?`

if [ -z "$STRESS" ]; then
	stress_message.sh
fi

exit $RESULT
