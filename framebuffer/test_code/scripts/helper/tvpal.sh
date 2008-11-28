#!/bin/sh

echo "TO PERFORM THIS TEST, IT IS NECESSARY THAT THE VIDEO OUTPUT IS CONNECTED TO ANY DEVICE THAT SUPPORTS PAL FORMAT"
sleep 5
$TESTBIN/fbmode 16
echo "Sending FB to output device"
sleep 3
echo tv > $TVOUT
sleep 1
echo "Changing output signal to PAL"
sleep 3
echo pal_bdghin > $TVSTD
$TESTBIN/fbout 2
sleep 2
$FBTEST

$TESTBIN/fbout 0
echo ntsc_m > $TVSTD
echo lcd > $TVOUT

if [ -z "$STRESS" ]; then
  echo "";echo "Was framebuffer performing all funtions fine in PAL format";echo ""
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

