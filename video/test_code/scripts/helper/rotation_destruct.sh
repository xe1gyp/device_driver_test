#!/bin/sh

VIDEO_PIPELINE=$1
SETIMG_PARAMETERS=$2
STREAMING_PARAMETERS=$3
SETWIN_PARAMETERS=$4
ANGLE=$5
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
# Usage: streaming <vid> <inputfile> [<n>]
$TESTBIN/streaming_tiler $VIDEO_PIPELINE $STREAMING_PARAMETERS 0

$TESTBIN/setrotation $VIDEO_PIPELINE 0

RESULT=`command_tracking.sh $RESULT $?`

if [ -z "$STRESS" ]; then
	stress_message.sh
fi

exit $RESULT
