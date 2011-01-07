#!/bin/sh

VID_PIPELINE=$1
MAX_X_DWNSCL=$2
MAX_Y_DWNSCL=$3

RESULT=0

#Usage: v4l2_ramdomcolorblocks <video_device>
$TESTBIN/v4l2_ramdomcolorblocks $VID_PIPELINE $MAX_X_DWNSCL $MAX_Y_DWNSCL
RESULT=`command_tracking.sh $RESULT $?`

if [ -z "$STRESS" ]; then
	stress_message.sh
fi

exit $RESULT
