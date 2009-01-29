#!/bin/sh

VIDEO_PIPELINE=$1
SETIMG_PARAMETERS=$2
STREAMING_PROCESS_PARAMETERS=$3
RESULT=0

# Usage: setimg <vid> <fmt> <width> <height>
$TESTBIN/setimg $VIDEO_PIPELINE $SETIMG_PARAMETERS
RESULT=`command_tracking.sh $RESULT $?`

# Usage: streaming <vid> <inputfile> [<n>]
$TESTBIN/streaming_process $VIDEO_PIPELINE $STREAMING_PROCESS_PARAMETERS
RESULT=`command_tracking.sh $RESULT $?`

if [ -z "$STRESS" ]; then
	stress_message.sh
fi

exit $RESULT
