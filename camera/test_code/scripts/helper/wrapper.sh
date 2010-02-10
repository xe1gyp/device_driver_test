#!/bin/sh
INSIZE=$1


if [ "$INSIZE" = "2592 1944" ]; then
  FIN="${TESTSCRIPT}/wrapperaux.raw"
  FNAME="${TMPBASE}/wrapperout_5MP.yuv"
fi

if [ "$INSIZE" = "3264 2464" ]; then
  FIN="${TESTSCRIPT}/8MP.raw"
  FNAME="${TMPBASE}/wrapperout_8MP.yuv"
fi

# Usage:prev_wrap <input_file> <input_width> <input_height> <output_file>

${TESTBIN}/prev_wrap $FIN $INSIZE $FNAME
RESULT=$?
echo "Test returned $RESULT"
sleep 1
chmod 744 $FNAME

if [ $RESULT -eq 255 ]; then
  ERR=1
elif [ -z "$STRESS" ]; then
  echo "";echo "Was preview wrapper capabled to create $FNAME file from $FIN with $INSIZE size?";echo ""
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

