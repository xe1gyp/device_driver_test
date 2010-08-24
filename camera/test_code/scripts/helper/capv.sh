#!/bin/sh

FPS=$1
SIZE=$2
FORMAT=$3
DEVICE=$4
TEST=$5

if [ "$TEST" = "ZOOM" ]; then
  time $TESTBIN/streaming_zoom $DEVICE $FORMAT $SIZE "1" "300"
  RESULT=$?
  echo "Test returned $RESULT"
else
  time $TESTBIN/streaming_frame $DEVICE $FPS $FORMAT $SIZE
  RESULT=$?
  echo "Test returned $RESULT"
  echo "Please report time that test takes to run."
fi

if [ $RESULT -eq 255 ]; then
  ERR=1
elif [ -z "$STRESS" ]; then
  if [ $SIZE = "VGA" -o $SIZE = "D1PAL" -o $SIZE = "D1NTSC" ]; then
    echo "";echo " Was streaming $FORMAT video at $FPS fps in $SIZE size displayed cleanly on LCD screen with video chopped to screen size.?";echo ""
  else 
    echo "";echo " Was streaming $FORMAT video at $FPS fps in $SIZE size displayed cleanly on LCD screen?";echo ""
  fi
  $WAIT_ANSWER
  ERR=$?
fi
if [ $ERR -eq 1 ]; then
  echo "FAIL"
  exit 1
else
  echo "PASS"
  exit 0
fi
