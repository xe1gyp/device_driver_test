 	#!/bin/sh

echo -n " Wait for Vsync using fb0 "
$TESTBIN/wait_for_vsync /dev/fb0
sleep 2

echo -n " Wait for Vsync using fb1 "
$TESTBIN/wait_for_vsync /dev/fb1
sleep 2

if [ -z "$STRESS" ]; then
	echo "";echo "Is the screen filled with different colors"
	echo "in $BPP_MODE bits mode and after that the screen "
	echo " filled with different color bars?";echo ""
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

