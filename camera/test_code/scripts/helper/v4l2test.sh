#!/bin/sh

MODE=$1
SIZE=$2
FORMAT=$3
DEVICE=$4

# Usage: ioctl <size> <format>
#$TESTBIN/ioctl $SIZE $FORMAT
#sleep 1 

#Usage: capture -d <dev> <mode>
#if [ $MODE = "read" ]; then
#  $TESTBIN/capture -d $DEVICE -r
if [ $MODE = "mmap" ]; then
  $TESTBIN/capture -d $DEVICE -m
elif [ $MODE = "user" ]; then
  $TESTBIN/capture -d $DEVICE -u
fi

if [ -z "$STRESS" ]; then
  echo "";echo " Did the V4L2 application work in $MODE mode without errors?";echo ""
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
