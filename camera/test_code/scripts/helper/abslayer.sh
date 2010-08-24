#!/bin/sh

DEVICE=$1
TEST=$2
OPTION=$3
MODE=$4

if [ -z "$MODE" ];then
   echo "Using default Mode = 1, before streamon"
   MODE=1
fi

$TESTBIN/streaming_abs $DEVICE $TEST $OPTION $MODE YUYV QVGA 1 100
RESULT=$?
echo "Test returned $RESULT"

if [ $RESULT -eq 255 ]; then
  ERR=1
elif [ -z "$STRESS" ]; then
  echo "";echo "Was the camera abstraction layer option changed succesfuly," \
	"and captured succesfuly without returning any" \
	"error message?";echo ""
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
