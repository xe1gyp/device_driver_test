#!/bin/sh

VIDEO_IN_PIPELINE=$1
STREAMING_PARAMETERS=$2
VIDEO_OUT_PIPELINE=$3
RESULT=0

# Usage: streaming_wb <vout_input_overlay> <inputfile> <vout_display_overlay>
$TESTBIN/streaming_wb $VIDEO_IN_PIPELINE $STREAMING_PARAMETERS $VIDEO_OUT_PIPELINE
RESULT=`command_tracking.sh $RESULT $?`

if [ -z "$STRESS" ]; then
	stress_message.sh
fi

exit $RESULT
