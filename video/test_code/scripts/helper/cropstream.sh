#!/bin/sh

VIDEO_PIPELINE=$1
SETCROP_PARAMETERS=$2
STREAMING_PARAMETERS=$3

# Usage: setcrop <vid> <left> <top> <width> <height>
$TESTBIN/setcrop $VIDEO_PIPELINE $SETCROP_PARAMETERS

# Usage: streaming <vid> <inputfile> [<n>]
$TESTBIN/streaming $VIDEO_PIPELINE $STREAMING_PARAMETERS

if [ -z "$STRESS" ]; then
	strees_messages.sh
fi
