#!/bin/sh

VIDEO_PIPELINE=$1
SETIMG_PARAMETERS=$2
STREAMING_PARAMETERS=$3
DEVICE=/sys/class/display_control/omap_disp_control/video$VIDEO_PIPELINE
TV_STANDARD_ENTRY=/sys/class/display_control/omap_disp_control/tv_standard
PREVIOUS_TV_STANDARD=`cat $TV_STANDARD_ENTRY`

echo tv > $DEVICE
echo pal_bdghi > $TV_STANDARD_ENTRY

# Usage: setimg <vid> <fmt> <width> <height>
$TESTBIN/setimg $VIDEO_PIPELINE $SETIMG_PARAMETERS

# Usage: streaming <vid> <inputfile> [<n>]
$TESTBIN/streaming $VIDEO_PIPELINE $STREAMING_PARAMETERS

# Reseting the previous values.
echo $PREVIOUS_TV_STANDARD > $TV_STANDARD_ENTRY
echo lcd > $DEVICE

if [ -z "$STRESS" ]; then
	strees_messages.sh
fi
