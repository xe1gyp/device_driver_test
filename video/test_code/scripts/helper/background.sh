#!/bin/sh

OUTPUT_DEVICE=$1
BG_COLOR=$2
RESULT=0

#Usage: setbgcolor <output device [1: LCD 2: TV]> <24 bit RGB value>
$TESTBIN/setbgcolor $OUTPUT_DEVICE $BG_COLOR
RESULT=`command_tracking.sh $RESULT $?`

if [ -z "$STRESS" ]; then
	stress_message.sh
fi

exit $RESULT
