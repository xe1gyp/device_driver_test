 	#!/bin/sh


if [ "$1" ]; then
  BPP_MODE=$1
  # Usage: fbmode <bits-per-pixel>
  $TESTBIN/fbmode $BPP_MODE
  sleep 2
else
  BPP_MODE="default"
fi

if [ "$2" ]; then
$FBDEV=$2
	echo "Yes.. $2 is present here "
	$TESTBIN/arm-none-linux-gnueabi-fbtest -f $2
else
	$TESTBIN/arm-none-linux-gnueabi-fbtest -f /dev/fb0
fi

if [ -z "$STRESS" ]; then
  echo "";echo "Was framebuffer performing all funtions fine in $BPP_MODE bits mode";echo ""
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
