#!/bin/sh

DEVICE=$1
TEST_TYPE=$2

if [ "$TEST_TYPE" = "LSC" ]; then
${TESTBIN}/snapshot -c $DEVICE -l 1 -n 450
fi

RESULT=$?
echo "Test returned $RESULT"

if [ $RESULT -eq 255 ]; then
  ERR=1
elif [ -z "$STRESS" ]; then
  echo ""; echo "Did you see the LSC pattern on the streaming video ?"
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
