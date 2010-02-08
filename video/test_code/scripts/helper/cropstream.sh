#!/bin/sh

VIDEO_PIPELINE=$1
SETCROP_PARAMETERS=$2
STREAMING_PARAMETERS=$3
SETIMG_PARAMETERS=$4
RESULT=0

if [ ! -z "$SETIMG_PARAMETERS" ]; then
		$TESTBIN/setimg $VIDEO_PIPELINE $SETIMG_PARAMETERS
		RESULT=`command_tracking.sh $RESULT $?`
	fi

# Usage: setcrop <vid> <left> <top> <width> <height>
$TESTBIN/setcrop $VIDEO_PIPELINE $SETCROP_PARAMETERS
RESULT=`command_tracking.sh $RESULT $?`

# Usage: streaming <vid> <inputfile> [<n>]
$TESTBIN/streaming_tiler $VIDEO_PIPELINE $STREAMING_PARAMETERS 0
RESULT=`command_tracking.sh $RESULT $?`

if [ -z "$STRESS" ]; then
	stress_message.sh
fi

exit $RESULT
