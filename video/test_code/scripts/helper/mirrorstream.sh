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
	echo "Was the video mirrored and streamed correctly?"
  $WAIT_ANSWER
  ERR=$?
  if [ $ERR -eq 1 ]; then
   echo "FAIL"
   exit 1
  else
   echo "PASS"
   exit 0
  fi
fi
