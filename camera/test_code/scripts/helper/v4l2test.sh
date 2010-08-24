#!/bin/sh

MODE=$1
DEVICE=$2

if [ $MODE = "mmap" ]; then
  $TESTBIN/capture -d $DEVICE -m
  RESULT=$?
  echo "Test returned $RESULT"
elif [ $MODE = "user" ]; then
  $TESTBIN/capture -d $DEVICE -u
  RESULT=$?
  echo "Test returned $RESULT"
fi

if [ $RESULT -eq 255 ]; then
  ERR=1
elif [ -z "$STRESS" ]; then
  echo "";echo "Did the V4L2 application work in $MODE mode without errors?";echo ""
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
