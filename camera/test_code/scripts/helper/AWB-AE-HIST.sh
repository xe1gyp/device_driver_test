#!/bin/sh

TEST=$1
PARAMETER=$2

if [ $TEST = "AWB" ]; then
  echo "";echo "Running h3a_stream program to configure the H3A engine and to collect auto white balance statistics."
   echo "";echo "To obtain again the AWB statistics, press S20."
  QUESTION="Was the camera able to capture video, and let you configure H3A engine and collect AWB statistics?"
  sleep 5
  #$TESTBIN/ioctl QCIF YUYV
  $TESTBIN/h3a_stream 1 30 1 $PARAMETER
  RESULT=$?
  echo "Test returned $RESULT"
elif [ $TEST = "AE" ]; then
  echo "";echo "Running h3a_stream program to configure the H3A engine and to collect auto exposure statistics."
  echo "";echo "To obtain again the AE/AWB statistics, press S20."
  QUESTION="Was the camera able to capture video, and let you configure H3A engine and collect AWB statistics?"
  sleep 5
  #$TESTBIN/ioctl QCIF YUYV
  $TESTBIN/h3a_stream 1 30 1 $PARAMETER
  RESULT=$?
  echo "Test returned $RESULT"
elif [ $TEST="HIST" ]; then
  echo "";echo "Run histogram test progam './hist_test'"
  echo "";echo "Check if the application run without problems"
  sleep 10
  $TESTBIN/hist_test
  RESULT=$?
  echo "Test returned $RESULT"
fi

if [ $RESULT -eq 255 ]; then
  ERR=1
elif [ -z "$STRESS" ]; then
  echo "";echo $QUESTION;echo ""
  $WAIT_ANSWER
  ERR=$?
fi
if [ $ERR -eq 1 ]; then
  echo "FAIL"
  exit 1
else
  echo "PASS"
  exit 0
fi

