#!/bin/sh

VIDEO_PIPELINE=$1
SETWIN_PARAMETERS=$2
STREAMING_PARAMETERS=$3

# Usage: setcrop <vid> <left> <top> <width> <height>
$TESTBIN/setcrop $VIDEO_PIPELINE 0 0 120 160

# Usage: setwin <vid> <left> <top> <width> <height>
$TESTBIN/setwin $VIDEO_PIPELINE $SETWIN_PARAMETERS

# Usage: streaming <vid> <inputfile> [<n>]
$TESTBIN/streaming $VIDEO_PIPELINE $STREAMING_PARAMETERS

if [ -z "$STRESS" ]; then
  echo "Was the window changed and video streamed correctly?"
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
