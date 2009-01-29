#!/bin/sh

VIDEO_PIPELINE=$1
SETIMG_PARAMETERS=$2
RESULT=0

# Usage: setimg <vid> <fmt> <width> <height>
$TESTBIN/setimg $VIDEO_PIPELINE $SETIMG_PARAMETERS
RESULT=`command_tracking.sh $RESULT $?`

exit $RESULT
