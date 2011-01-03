#!/bin/sh

VIDEO_PIPELINE=$1
SETIMG_PARAMETERS=$2
ROT_ANGLE=$3
RESULT=0

# Usage: setimg <vid> <fmt> <width> <height>
$TESTBIN/setimg $VIDEO_PIPELINE $SETIMG_PARAMETERS

# Usage: setrotation <vid> <angle>
$TESTBIN/setrotation $VIDEO_PIPELINE $ROT_ANGLE
RESULT=`command_tracking.sh $RESULT $?`

if [ -z "$STRESS" ]; then
	stress_message.sh
fi

exit $RESULT

