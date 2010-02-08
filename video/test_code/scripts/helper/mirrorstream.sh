#!/bin/sh

VIDEO_PIPELINE=$1
SETMIRROR_PARAMETERS=$2
STREAMING_PARAMETERS=$3
MESSAGE=$4
RESULT=0

sleep $MESSAGE_DELAY

# Usage: mirroring <vid> <enable[1]/disable[0]>
$TESTBIN/setmirror $VIDEO_PIPELINE $SETMIRROR_PARAMETERS
RESULT=`command_tracking.sh $RESULT $?`

# Usage: streaming <vid> <inputfile> [<n>]
$TESTBIN/streaming_tiler $VIDEO_PIPELINE $STREAMING_PARAMETERS 0
RESULT=`command_tracking.sh $RESULT $?`

if [ -z "$STRESS" ]; then
	stress_message.sh
fi

exit $RESULT
