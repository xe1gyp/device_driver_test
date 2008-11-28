#!/bin/sh

TEST=$1
$TESTBIN/fbmode 16

if [ $TEST = "1" ]; then 
  echo -n 3 > $FBPWD
  echo "FB suspended"
  sleep 2
  $FBTEST
  MSG="Did the test passed without kernel crash or hang and the framebuffer become blank and inoperable?"
elif [ $TEST = "2" ]; then
  echo -n 3 > $FBPWD
  echo "FB suspended"
  sleep 2
  echo -n 0 > $FBPWD
  echo "FB resumed"
  sleep 2
  $FBTEST
  MSG="Did the test passed without kernel crash or hang and the framebuffer become blank and inoperable when it suspend, and after that the framebuffer unblanked and the screen restored the test suite passed all tests with proper display on the output screen?"
elif [ $TEST = "3" ]; then
  echo "IF YOUR BOARD WAS NOT BOOTED USING ROTATION (video=omap24xxfb:rotation=0) THEN ABORT THE SCRIPT"
  sleep 6
  echo -n 3 > $FBPWD
  echo "FB suspended"
  sleep 2
  echo -n 0 > $FBPWD
  echo "FB resumed"
  sleep 2
  $FBTEST
  MSG="Did the test passed without kernel crash or hang and the framebuffer become blank and inoperable when it suspend, and after that the framebuffer unblanked and the screen restored the test suite passed all tests with proper display on the output screen?"
elif [ $TEST = "4" ]; then
  echo -n 3 > $FBPWD
  echo "FB suspended"
  sleep 2
  echo -n 3 > $FBPWD
  echo "FB suspended"
  sleep 2
  echo -n 0 > $FBPWD
  echo "FB resumed"
  sleep 2
  MSG="Did the framebuffer remains blank and inoperable and without kernel crash or hang when it already is suspended?"
elif [ $TEST = "5" ]; then
   echo -n 3 > $FBPWD
  echo "FB suspended"
  sleep 2
  echo -n 0 > $FBPWD
  echo "FB resumed"
  sleep 2
  echo -n 0 > $FBPWD
  echo "FB resumed"
  sleep 2
  MSG="Did the framebuffer is operable and without kernel crash or hang when it already is resumed?"
elif [ $TEST = "6" ]; then
  echo -n 3 > $FBPWD
  echo "FB suspended"
  echo -n 0 > $FBPWD
  echo "FB resumed"
  echo -n 3 > $FBPWD
  echo "FB suspended"
  sleep 1
  echo -n 0 > $FBPWD
  echo "FB resumed"
  sleep 1
  echo -n 3 > $FBPWD
  echo "FB suspended"
  sleep 2
  echo -n 0 > $FBPWD
  echo "FB resumed"
  sleep 2
  MSG="Did the framebuffer 'suspend' and 'resume' per the commands run without kernel crash or hang?"
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

