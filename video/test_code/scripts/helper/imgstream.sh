#!/bin/sh

VIDEO_PIPELINE=$1
SETIMG_PARAMETERS=$2
STREAMING_PARAMETERS=$3

# Usage: setimg <vid> <fmt> <width> <height>
$TESTBIN/setimg $VIDEO_PIPELINE $SETIMG_PARAMETERS

# Usage: streaming <vid> <inputfile> [<n>]
$TESTBIN/streaming $VIDEO_PIPELINE $STREAMING_PARAMETERS

if [ -z "$STRESS" ]; then
  echo "";echo "Was the video streamed correctly?";echo ""
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
