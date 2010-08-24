#!/bin/sh

EFFECT=$1
SIZE=$2
FORMAT=$3
DEVICE=$4

if [ $EFFECT = "contrast" ]; then
  $TESTBIN/streaming_to_video_file $DEVICE $FORMAT $SIZE 1 c
  RESULT=$?
  echo "Test returned $RESULT"
elif [ $EFFECT = "brightness" ]; then
  $TESTBIN/streaming_to_video_file $DEVICE $FORMAT $SIZE 1 b
  RESULT=$?
  echo "Test returned $RESULT"
elif [ $EFFECT = "color" ]; then
  $TESTBIN/streaming_to_video_file $DEVICE $FORMAT $SIZE 1 e
  RESULT=$?
  echo "Test returned $RESULT"
fi

if [ $RESULT -eq 255 ]; then
  ERR=1
elif [ -z "$STRESS" ]; then
  if [ $EFFECT = "color" ]; then
    echo "";echo "Did the captured images got displayed cleanly on LCD screen with color effect changing between normal, sepia, and black and white?";
  else
    echo "";echo "Did you notice the $EFFECT changing from the lowest to the highest.";echo ""
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
