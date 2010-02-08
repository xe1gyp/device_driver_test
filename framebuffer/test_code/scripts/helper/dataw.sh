 	#!/bin/sh


if [ "$1" ]; then
  BPP_MODE=$1
  # Usage: fbmode <bits-per-pixel>
  $TESTBIN/fbmode $BPP_MODE
  sleep 2
else
  BPP_MODE="default"
fi

$TESTBIN/fbtest  -w -l 0xCA800 -f 0x0fab
echo "====7===="
sleep 2
$TESTBIN/fbtest  -w -l 0xCA800 -f 0x0bad
echo "====6===="
sleep 2
$TESTBIN/fbtest  -w -l 0xCA800 -f 0x0add
echo "====5===="
sleep 2
$TESTBIN/fbtest  -w -l 0xCA800 -f 0x0fff
echo "====4===="
sleep 2
$TESTBIN/fbtest  -w -l 0xCA800 -f 0x0000
echo "====3===="
sleep 2
$TESTBIN/fbtest  -w -l 0xCA800 -f 0x0ff0
echo "====2===="
sleep 2
$TESTBIN/fbtest  -w -l 0xCA800 -f 0x00f2
echo "====1===="
sleep 2

$TESTBIN/fb_user_tests
sleep 4

if [ -z "$STRESS" ]; then
  echo "";echo "Is the screen filled with different colors in $BPP_MODE bits mode and after that the screen filled with different color bars?";echo ""
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

