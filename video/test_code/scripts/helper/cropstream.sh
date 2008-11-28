#!/bin/sh

VIDEO_PIPELINE=$1
SETCROP_PARAMETERS=$2
STREAMING_PARAMETERS=$3
WIN_PARAMETERS=$4

$TESTBIN/setwin $VIDEO_PIPELINE $WIN_PARAMETERS

# Usage: setcrop <vid> <left> <top> <width> <height>
$TESTBIN/setcrop $VIDEO_PIPELINE $SETCROP_PARAMETERS

# Usage: streaming <vid> <inputfile> [<n>]
$TESTBIN/streaming $VIDEO_PIPELINE $STREAMING_PARAMETERS

if [ -z "$STRESS" ]; then
  echo "Was the video cropped and streamed correctly?"
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
