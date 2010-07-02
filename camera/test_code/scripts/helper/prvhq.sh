#!/bin/sh

ISIZE=$1
DEVICE=$2
TEST=$3
TESTPARAM=$4

if [ "$ISIZE" = "8MP" ]; then
  INSIZE="-iw 3264 -ih 2464"
  INFILE="-ifile ${TESTSCRIPT}/8MP.raw"
fi

if [ "$ISIZE" = "5MP" ]; then
  INSIZE="-iw 2592 -ih 1944"
  INFILE="-ifile ${TESTSCRIPT}/5MP.raw"
fi

if [ "$ISIZE" = "XGA" ]; then
  INSIZE="-iw 1024 -ih 768"
  INFILE="-ifile ${TESTSCRIPT}/XGA.raw"
fi

if [ "$ISIZE" = "VGA" ]; then
  INSIZE="-iw 640 -ih 480"
  INFILE="-ifile ${TESTSCRIPT}/VGA.raw"
fi

OUTFILE="${TMPBASE}/prvhq_out_${TEST}-${TESTPARAM}-${ISIZE}.yuv"

DEVICE="-idev /dev/$DEVICE"

if [ "$TEST" = "CFA" ]; then
  TESTPARAM="disabled"
  OUTFILE="${TMPBASE}/prvhq_out_${TEST}-${TESTPARAM}-${ISIZE}.yuv"
  $TESTBIN/prvhq $INSIZE $INFILE $DEVICE -ofile $OUTFILE -turnoff_cfa
  RESULT=$?
  echo "Test returned $RESULT"
elif [ "$TEST" = "BRIGHTNESS" ]; then
  $TESTBIN/prvhq $INSIZE $INFILE $DEVICE -ofile $OUTFILE -brightness $TESTPARAM
  RESULT=$?
  echo "Test returned $RESULT"
elif [ "$TEST" = "CONTRAST" ]; then
  $TESTBIN/prvhq $INSIZE $INFILE $DEVICE -ofile $OUTFILE -contrast $TESTPARAM
  RESULT=$?
  echo "Test returned $RESULT"
elif [ "$TEST" = "NF" ]; then
  TESTPARAM="disabled"
  OUTFILE="${TMPBASE}/prvhq_out_${TEST}-${TESTPARAM}-${ISIZE}.yuv"
  $TESTBIN/prvhq $INSIZE $INFILE $DEVICE -ofile $OUTFILE -turnoff_nf
  RESULT=$?
  echo "Test returned $RESULT"
elif [ "$TEST" = "WBAL" ]; then
  $TESTBIN/prvhq $INSIZE $INFILE $DEVICE -ofile $OUTFILE -wbal $TESTPARAM
  RESULT=$?
  echo "Test returned $RESULT"
fi

chmod 744 $OUTFILE

ERR=0

if [ $RESULT -eq 255 ]; then
  ERR=1
elif [ -z "$STRESS" ]; then

if [ "$TEST" = "INFO" ]; then
  echo "";echo "Was the displayed information for $INDEVICE correct?";echo ""
else
  echo "";echo "Was camera able to capture $FRNUM frame(s) in $FORMAT format of $SIZE size and saved into $FNAME?";echo ""
fi
  $WAIT_ANSWER
  ERR=$?
fi

if [ $ERR -eq 1 ]; then
  #color output
  echo -en "\033[0;31m FAIL \033[0m \n"
  #echo "FAIL"
  exit 1
else
  echo -en "\033[0;32m PASS \033[0m \n"
  #echo "PASS"
  exit 0
fi

