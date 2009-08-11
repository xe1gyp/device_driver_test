#!/bin/sh

TEST=$1
OPTION=$2
MODE=$3

if [ -z "$MODE" ];then
   echo "Using defualt Mode = 1, before streamon"
   MODE=1
fi

# Usage: Test_Case_xxx <vid> <option>
#if [ "$TEST" = "CCDC" ];then
#	$TESTBIN/Test_Case_ccdc5 1 $OPTION
#fi

#if [ "$TEST" = "PREVIEW" ]; then
#        $TESTBIN/Test_Case_preview5 1 $OPTION
#fi

#if [ "$TEST" = "TABLES" ];then
#	if [ "$OPTION" = "lsc" ];then
#        	$TESTBIN/Test_Case_ccdc5 1 "lc"
#	fi
#        $TESTBIN/Test_Case_tables5 1 $OPTION
#fi

# Usage: streaming_abs [camDevice] [absLayerGroup] [absLayerOption] [absLayerMode] [pixelFormat] [<sizeW> <sizeH>] [(vid)] [<count>] [<file>]
#if [ "$OPTION" = "bcl" ]; then
#	$TESTBIN/streaming_abs 1 $TEST $OPTION 1 $MODE YUYV QVGA 1 100
#else 
	#Using UYVY provisionally. It is necesary to use with image analyzer software.
        $TESTBIN/streaming_abs 1 $TEST $OPTION $MODE YUYV QVGA 1 100
	#$TESTBIN/streaming_abs 1 $TEST $OPTION $MODE UYVY VGA 1 1 $TMPBASE/VGA_"$OPTION".yuv
	RESULT=$?
	echo "Test returned $RESULT"
#fi

#chmod 744 $TMPBASE/VGA_"$OPTION".yuv

if [ $RESULT -eq 255 ]; then
  ERR=1
elif [ -z "$STRESS" ]; then
  echo "";echo "Was the camera abstraction layer option changed succesful," \
	"and captured succesfuly without returning any" \
	"error message?";echo ""
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
