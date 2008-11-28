#!/bin/sh
TESTYPE=$1

# Usage:prev_wrap <input_file> <input_width> <input_height> <output_file>
  FIN="${TESTSCRIPT}/wrapperaux.raw"
  FNAME="${TMPBASE}/wrapperout.yuv"
  INSIZE="2592 1944"
${TESTBIN}/prev_wrap $FIN $INSIZE $FNAME
sleep 1
chmod 744 $FNAME

if [ -z "$STRESS" ]; then
  echo "";echo "Was preview wrapper capabled to create $FNAME file from $FIN with $INSIZE size?";echo ""
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

