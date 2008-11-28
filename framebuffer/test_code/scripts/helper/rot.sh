#!/bin/sh

STATE=$1

if [ $STATE = "1" ]; then
  MSG="Please boot the board adding the next arguments 'video=omap24xxfb:rotation=0' and run run linux-fbdev's 'fbtest' suite"
elif [ $STATE = "2" ]; then
  MSG="Please boot the board adding the next arguments 'video=omap24xxfb:rotation=90' and run run linux-fbdev's 'fbtest' suite"
elif [ $STATE = "3" ]; then
  MSG="Please boot the board adding the next arguments 'video=omap24xxfb:rotation=180' and run run linux-fbdev's 'fbtest' suite"
elif [ $STATE = "4" ]; then
  MSG="Please boot the board adding the next arguments 'video=omap24xxfb:rotation=270' and run run linux-fbdev's 'fbtest' suite"
elif [ $STATE = "5" ]; then
  echo "DO THIS TEST WITH BOARD BOOTED WITH SOME OTHER ROTATION THAT DEFAULT"
  sleep 5
  $TESTBIN/fbrotate 0
  $FBTEST
  MSG="Did the test suite passed all tests with proper display on the output screen?"
elif [ $STATE = "6" ]; then
  echo "DO THIS TEST WITH BOARD BOOTED WITH SOME OTHER ROTATION THAT DEFAULT"
  sleep 5
  $TESTBIN/fbrotate 90
  $FBTEST
  MSG="Did the test suite passed all tests with proper display on the output screen?"
elif [ $STATE = "7" ]; then
  echo "DO THIS TEST WITH BOARD BOOTED WITH SOME OTHER ROTATION THAT DEFAULT"
  sleep 5
  $TESTBIN/fbrotate 180
  $FBTEST
  MSG="Did the test suite passed all tests with proper display on the output screen?"
elif [ $STATE = "8" ]; then
  echo "DO THIS TEST WITH BOARD BOOTED WITH SOME OTHER ROTATION THAT DEFAULT"
  sleep 5
  $TESTBIN/fbrotate 270
  $FBTEST
  MSG="Did the test suite passed all tests with proper display on the output screen?"
fi

if [ -z "$STRESS" ]; then
  echo "";echo "$MSG"; echo ""
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
