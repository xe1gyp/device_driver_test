#!/bin/sh

TEST=$1
PARAMETER=$2
DEVICE=$3

if [ -z "$DEVICE" ]; then
  echo "Defaulting to use Device 1"
  DEVICE=1
fi

if [ $TEST = "AWB" ]; then
  echo "";echo "Running h3a_stream program to configure the H3A engine and to collect auto white balance statistics."
   echo "";echo "To obtain again the AWB statistics, press S20."
  QUESTION="Was the camera able to capture video, and let you configure H3A engine and collect AWB statistics?"
  sleep 5
  $TESTBIN/h3a_stream $DEVICE 30 1 $PARAMETER
  RESULT=$?
  echo "Test returned $RESULT"
elif [ $TEST = "AE" ]; then
  echo "";echo "Running h3a_stream program to configure the H3A engine and to collect auto exposure statistics."
  echo "";echo "To obtain again the AE/AWB statistics, press S20."
  QUESTION="Was the camera able to capture video, and let you configure H3A engine and collect AWB statistics?"
  sleep 5
  $TESTBIN/h3a_stream $DEVICE 30 1 $PARAMETER
  RESULT=$?
  echo "Test returned $RESULT"
elif [ $TEST = "HIST" ]; then
  echo "";echo "Run histogram test progam './hist_test'"
  echo "";echo "Check if the application run without problems"
  sleep 5
  $TESTBIN/hist_test $DEVICE
  RESULT=$?
  echo "Test returned $RESULT"
elif [ $TEST = "I421_ERRATA" ]; then
  echo "";echo "The test application waits for AF and H3A statistics.";
  echo "It should be in compliance with errata i421."
  echo "The AF and H3A should be displayed without problems."
  sleep 6
  $TESTBIN/i421_errata_af_stats
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

