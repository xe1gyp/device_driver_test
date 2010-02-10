#!/bin/sh

EFFECT=$1
SIZE=$2
FORMAT=$3
DEVICE=$4

# Usage: ioctl <size> <format>
#$TESTBIN/ioctl $SIZE $FORMAT
#sleep 1 

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
    echo "";echo "Do you observe that captured image is displayed cleanly on LCD screen with $EFFECT effect changing from normal, to sepia to black and white?";
	echo "";echo "Do you notice the $EFFECT changing from the lowest to the highest.";echo ""
  else
    echo "";echo "Do you notice the $EFFECT changing from the lowest to the highest.";echo ""
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
