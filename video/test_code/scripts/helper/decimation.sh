#!/bin/sh

VIDEO_PIPELINE=$1
WIDTH=$2
HEIGTH=$3
PIXEL_FORMAT=$4
VIDEO_FILE=$5
SETWIN_PARAMETERS=$6

# Usage: setimg <video> <pixel Fmt> <width> <heigth>
$TESTBIN/setimg $VIDEO_PIPELINE $PIXEL_FORMAT $WIDTH $HEIGTH

# Usage: setcrop <vid> <left> <top> <width> <height>
$TESTBIN/setcrop $VIDEO_PIPELINE 0 0 $WIDTH $HEIGTH

# Usage: setwin <vid> <left> <top> <width> <height>
$TESTBIN/setwin $VIDEO_PIPELINE $SETWIN_PARAMETERS

# Usage: streaming <vid> <inputfile> [<n>]
$TESTBIN/streaming $VIDEO_PIPELINE $VIDEO_FILE

if [ -z "$STRESS" ]; then
	strees_messages.sh
fi

