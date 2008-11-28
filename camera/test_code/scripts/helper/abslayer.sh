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
if [ "$OPTION" = "bcl" ]; then
	$TESTBIN/streaming_abs 1 $TEST $OPTION 1 $MODE YUYV QVGA 1 100
else 
        $TESTBIN/streaming_abs 1 $TEST $OPTION $MODE YUYV QVGA 1 100
fi

if [ -z "$STRESS" ]; then
  echo "";echo "Was camera the abstraction layer option changed succesful, \
	and captured and displayed the video on the LCD without any crash or\
	error message?";echo ""
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
