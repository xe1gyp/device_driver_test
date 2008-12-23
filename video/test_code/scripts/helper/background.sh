#!/bin/sh

OUTPUT_DEVICE=$1
BG_COLOR=$2

#Usage: setbgcolor <output device [1: LCD 2: TV]> <24 bit RGB value>
$TESTBIN/setbgcolor $OUTPUT_DEVICE $BG_COLOR

if [ -z "$STRESS" ]; then
	strees_messages.sh
fi
