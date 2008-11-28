#!/bin/sh

STATE=$1

if [ $STATE = "unblank" ]; then
  #Usage: fbtest -u to enable, -b to disable
  $TESTBIN/fbtest -u
  $TESTBIN/fbtest -u
elif [ $STATE = "blank" ]; then
  $TESTBIN/fbtest -b
  $TESTBIN/fbtest -b
fi

if [ -z "$STRESS" ]; then
  echo "";echo "Did the the framebuffer go $STATE?"
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
