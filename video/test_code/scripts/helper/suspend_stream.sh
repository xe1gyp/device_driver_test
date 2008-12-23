#!/bin/sh

VIDEO_PIPELINE=$1
SETIMG_PARAMETERS=$2
STREAMING_PARAMETERS=$3
SETCROP_PARAMETERS=$4
SETWIN_PARAMETERS=$5

# Usage: setimg <vid> <fmt> <width> <height>
$TESTBIN/setimg $VIDEO_PIPELINE $SETIMG_PARAMETERS

if [ ! -z "$SETCROP_PARAMETERS" ]; then
	# Usage: setcrop <vid> <left> <top> <width> <height>
	$TESTBIN/setcrop $VIDEO_PIPELINE $SETCROP_PARAMETERS
fi

if [ ! -z "$SETWIN_PARAMETERS" ]; then
	# Usage: setwin <vid> <left> <top> <width> <height>
	$TESTBIN/setwin $VIDEO_PIPELINE $SETWIN_PARAMETERS
fi


# Usage: streaming <vid> <inputfile> [<n>]
$TESTBIN/streaming $VIDEO_PIPELINE $STREAMING_PARAMETERS 1&
sleep 1
echo -n mem > /sys/power/state

if [ -z "$STRESS" ]; then
	strees_messages.sh
fi
