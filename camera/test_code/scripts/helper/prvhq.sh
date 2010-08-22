#!/bin/sh

ISIZE=$1
DEVICE=$2
TEST=$3
TESTPARAM=$4

if [ "$TEST" = "CFA" ]; then
	TESTPARAM="disabled"
elif [ "$TEST" = "NF" ]; then
	TESTPARAM="disabled"
fi

if [ "$ISIZE" = "8MP" ]; then
  INSIZE="-iw 3264 -ih 2464"
  INFILE="-ifile ${TESTSCRIPT}/8MP.raw"
  OUTFILE="${TMPBASE}/prvhq_out_${TEST}-${TESTPARAM}-${ISIZE} _3248x2456_UYVY.yuv"
fi

if [ "$ISIZE" = "5MP" ]; then
  INSIZE="-iw 2592 -ih 1944"
  INFILE="-ifile ${TESTSCRIPT}/5MP.raw"
  OUTFILE="${TMPBASE}/prvhq_out_${TEST}-${TESTPARAM}-${ISIZE}_2576x1936_UYVY.yuv"
fi

if [ "$ISIZE" = "XGA" ]; then
  INSIZE="-iw 1024 -ih 768"
  INFILE="-ifile ${TESTSCRIPT}/XGA.raw"
  OUTFILE="${TMPBASE}/prvhq_out_${TEST}-${TESTPARAM}-${ISIZE}_1008x760_UYVY.yuv"
fi

if [ "$ISIZE" = "VGA" ]; then
  INSIZE="-iw 640 -ih 480"
  INFILE="-ifile ${TESTSCRIPT}/VGA.raw"
  OUTFILE="${TMPBASE}/prvhq_out_${TEST}-${TESTPARAM}-${ISIZE}_624x472_UYVY.yuv"
fi

DEVICE="-idev /dev/$DEVICE"

if [ "$TEST" = "CFA" ]; then
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
  echo "";echo "Was camera able to convert $ISIZE.raw to $ISIZE.yuv\
  and save into $OUTFILE?";echo ""
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

