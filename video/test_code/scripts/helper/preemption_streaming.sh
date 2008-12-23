#!/bin/sh

VIDEO_PIPELINE=$1
SETIMG_PARAMETERS=$2
STREAMING_PROCESS_PARAMETERS=$3

# Usage: setimg <vid> <fmt> <width> <height>
$TESTBIN/setimg $VIDEO_PIPELINE $SETIMG_PARAMETERS

# Usage: streaming <vid> <inputfile> [<n>]
$TESTBIN/streaming_process $VIDEO_PIPELINE $STREAMING_PROCESS_PARAMETERS


if [ -z "$STRESS" ]; then
	strees_messages.sh
fi
