#!/bin/sh

$TESTBIN/fbmode 8
DITHERING=$1

if [ $DITHERING = "ON" ]; then
  echo 'on' > $DITHE
  echo 'on' > $DITHE
  echo "Dithering ON"
  sleep 2
elif [ $DITHERING = "OFF" ]; then
  echo 'off' > $DITHE
  echo 'off' > $DITHE
  echo "Dithering OFF"
  sleep 2
fi

$FBTEST

$TESTBIN/fbmode 16
if [ -z "$STRESS" ]; then
  echo "";echo "Is the image displayed properly and without kernel crash with dithering $DITHERING?"; echo ""
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

