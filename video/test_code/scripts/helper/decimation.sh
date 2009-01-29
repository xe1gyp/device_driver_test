#!/bin/sh

VIDEO_PIPELINE=$1
WIDTH=$2
HEIGTH=$3
PIXEL_FORMAT=$4
VIDEO_FILE=$5
SETWIN_PARAMETERS=$6
RESULT=0

# Usage: setimg <video> <pixel Fmt> <width> <heigth>
$TESTBIN/setimg $VIDEO_PIPELINE $PIXEL_FORMAT $WIDTH $HEIGTH
RESULT=`command_tracking.sh $RESULT $?`

# Usage: setcrop <vid> <left> <top> <width> <height>
$TESTBIN/setcrop $VIDEO_PIPELINE 0 0 $WIDTH $HEIGTH
RESULT=`command_tracking.sh $RESULT $?`

# Usage: setwin <vid> <left> <top> <width> <height>
$TESTBIN/setwin $VIDEO_PIPELINE $SETWIN_PARAMETERS
RESULT=`command_tracking.sh $RESULT $?`

# Usage: streaming <vid> <inputfile> [<n>]
$TESTBIN/streaming $VIDEO_PIPELINE $VIDEO_FILE
RESULT=`command_tracking.sh $RESULT $?`

if [ -z "$STRESS" ]; then
	stress_message.sh
fi

exit $RESULT
