#!/bin/sh

VIDEO_PIPELINE=$1
SETMIRROR_PARAMETERS=$2
STREAMING_PARAMETERS=$3
MESSAGE=$4

sleep $MESSAGE_DELAY

# Usage: mirroring <vid> <enable[1]/disable[0]>
$TESTBIN/setmirror $VIDEO_PIPELINE $SETMIRROR_PARAMETERS

# Usage: streaming <vid> <inputfile> [<n>]
$TESTBIN/streaming $VIDEO_PIPELINE $STREAMING_PARAMETERS

if [ -z "$STRESS" ]; then
	strees_messages.sh
fi
