#!/bin/sh

VIDEO_PIPELINE=$1
DISPLAY_PARAMETERS=$2
RESULT=0

# Usage: v4l2_rolling_bars <vid> <width> <height>
$TESTBIN/v4l2_rolling_bars $VIDEO_PIPELINE $DISPLAY_PARAMETERS

if [ -z "$STRESS" ]; then
	stress_message.sh
fi

exit $RESULT
