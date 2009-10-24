#!/bin/sh

BG_COLOR=$1
RESULT=0

#Usage: setbgcolor <output device [1: LCD 2: TV]> <24 bit RGB value>
$TESTBIN/setbgcolor $BG_COLOR
RESULT=`command_tracking.sh $RESULT $?`

if [ -z "$STRESS" ]; then
	stress_message.sh
fi

exit $RESULT
