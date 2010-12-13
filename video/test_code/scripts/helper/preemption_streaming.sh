#!/bin/sh

VIDEO_PIPELINE=$1
SETIMG_PARAMETERS=$2
SETWIN_PARAMETERS=$3
STREAMING_PROCESS_PARAMETERS=$4
RESULT=0


# Usage: setimg <vid> <fmt> <width> <height>
$TESTBIN/setrotation $VIDEO_PIPELINE 0
RESULT=`command_tracking.sh $RESULT $?`

# Usage: setimg <vid> <fmt> <width> <height>
$TESTBIN/setimg $VIDEO_PIPELINE $SETIMG_PARAMETERS
RESULT=`command_tracking.sh $RESULT $?`

# Usage: setwin <vid> <left> <top> <width> <height>
$TESTBIN/setwin $VIDEO_PIPELINE $SETWIN_PARAMETERS
RESULT=`command_tracking.sh $RESULT $?`

# Usage: streaming <vid> <inputfile> [<n>]
$TESTBIN/streaming_process $VIDEO_PIPELINE $STREAMING_PROCESS_PARAMETERS
RESULT=`command_tracking.sh $RESULT $?`

if [ -z "$STRESS" ]; then
	stress_message.sh
fi

exit $RESULT
