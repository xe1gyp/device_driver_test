#!/bin/sh

  $TESTBIN/fbmode 16

  echo ""; echo "Changing FB display size according to multiplication factors: 1/2"; echo ""
  sleep 3
  $TESTBIN/fbsize x 1 2
  echo ""; echo "Display size changed to 400 x 240"; echo ""
  sleep 2
  $FBTEST
  
  echo ""; echo "Changing FB display size according to multiplication factors: 4/3"; echo ""
  $TESTBIN/fbsize x 4 3
  echo ""; echo "Display size changed to 533 x 320"; echo ""
  sleep 2
  $FBTEST

  echo ""; echo "Changing FB display size according to multiplication factors: 3/4"; echo ""
  $TESTBIN/fbsize x 3 4
  echo ""; echo "Display size changed to 399 x 240"; echo ""
  sleep 2
  $FBTEST

  echo ""; echo "Changing FB display size according to multiplication factors: 2/1"; echo ""
  $TESTBIN/fbsize x 2 1
  echo ""; echo "Display size changed to 798 x 480"; echo ""
  sleep 2
  $FBTEST

if [ -z "$STRESS" ]; then
  echo "";echo "Has the FB display size changed according to the multiplication factors given by the numerator and denominator parameters to the 'fbsize' program?"; echo ""
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
