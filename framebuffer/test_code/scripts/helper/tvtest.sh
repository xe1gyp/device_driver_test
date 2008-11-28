#!/bin/sh

echo "TO PERFORM THIS TEST, IT IS NECESSARY THAT THE VIDEO OUTPUT IS CONNECTED TO TV"
sleep 5
$TESTBIN/fbmode 16
echo "Sending FB to output device"
sleep 3
echo tv > $TVOUT
echo "Changing output signal to NTSC"
sleep 3
$TESTBIN/fbout 1
sleep 2

echo "";echo "Scenario L_DD_FB_0002";echo ""
sleep 5
confbpp.sh 8
confbpp.sh 16
confbpp.sh 24

echo "";echo "Scenario L_DD_FB_0005";echo ""
sleep 5
sizechg.sh

echo "";echo "Scenario L_DD_FB_0006";echo ""
sleep 5
mirror.sh

echo "";echo "Scenario L_DD_FB_0007";echo ""
sleep 5
fbflipfb.sh

echo "";echo "Scenario L_DD_FB_0010";echo ""
sleep 5
rot.sh "1"
rot.sh "2"
rot.sh "3"
rot.sh "4"

echo "";echo "Scenario L_DD_FB_1003";echo ""
sleep 5
fbio.sh "blank"
fbio.sh "unblank"

echo "";echo "Scenario L_DD_FB_1004";echo ""
sleep 5
rotmir.sh "0"
rotmir.sh "90"
rotmir.sh "180"
rotmir.sh "270"


$TESTBIN/fbout 0
echo ntsc_m > $TVSTD
echo lcd > $TVOUT

if [ -z "$STRESS" ]; then
  echo "";echo "Was framebuffer performing all funtions fine in TV output";echo
 ""
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

