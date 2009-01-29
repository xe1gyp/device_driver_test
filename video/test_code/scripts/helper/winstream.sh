#!/bin/sh

VIDEO_PIPELINE=$1
SETWIN_PARAMETERS=$2
STREAMING_PARAMETERS=$3
RESULT=0

# Usage: setcrop <vid> <left> <top> <width> <height>
$TESTBIN/setcrop $VIDEO_PIPELINE 0 0 120 160
RESULT=`command_tracking.sh $RESULT $?`

# Usage: setwin <vid> <left> <top> <width> <height>
$TESTBIN/setwin $VIDEO_PIPELINE $SETWIN_PARAMETERS
RESULT=`command_tracking.sh $RESULT $?`

# Usage: streaming <vid> <inputfile> [<n>]
$TESTBIN/streaming $VIDEO_PIPELINE $STREAMING_PARAMETERS
RESULT=`command_tracking.sh $RESULT $?`

if [ -z "$STRESS" ]; then
	stress_message.sh
fi

exit $RESULT
