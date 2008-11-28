#!/bin/sh

TEST=$1

if [ $TEST = "AWB" ]; then
  echo "";echo "Running h3a_stream program to configure the H3A engine and to collect auto white balance statistics."
  echo "";echo "To change WB gains, follow the below table, pressing the indicated key + ENTER:"
  echo "'y' to increase digital gain (16 units in all cases)"
  echo "'h' to decrease digital gain"
  echo "'u' to increase blue gain";
  echo "'j' to decrease blue gain"
  echo "'i' to increase red gain"
  echo "'k' to decrease red gain"
  echo "'o' to increase green-blue gain"
  echo "'l' to decrease green-blue gain"
  echo "'p' to increase green-red gain"
  echo "';' to decrease green-red gain"
  echo "";echo "To obtain again the AWB statistics, press '1' + ENTER."
  QUESTION="Was the camera able to capture video, and let you configure H3A engine and collect AWB statistics?"
  sleep 5
  #$TESTBIN/ioctl QCIF YUYV
  $TESTBIN/h3a_stream
elif [ $TEST = "AE" ]; then
  echo "";echo "Running h3a_stream program to configure the H3A engine and to collect auto exposure statistics."
  echo "";echo "To change auto exposure parameters, follow the below table, pressing the indicated key + ENTER:"
  echo "";echo "'r' to increase shutter time (+ 2000 ns)"
  echo "";echo "'f' to decrease shutter time (- 2000 ns)"
  echo "";echo "'t' to increase analog gain (+ 4 units)"
  echo "";echo "'g' to decrease analog gain (- 4 units)"
  echo "";echo "To obtain again the AE/AWB statistics, press '1' + ENTER."
  QUESTION="Was the camera able to capture video, and let you configure H3A engine and collect AWB statistics?"
  sleep 5
  #$TESTBIN/ioctl QCIF YUYV
  $TESTBIN/h3a_stream
elif [ $TEST="HIST" ]; then
  echo "";echo "Run histogram test progam './hist_test'"
  echo "";echo "Check if the application run without problems"
  sleep 10
  $TESTBIN/hist_test
fi



if [ -z "$STRESS" ]; then
  echo "";echo $QUESTION;echo ""
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

