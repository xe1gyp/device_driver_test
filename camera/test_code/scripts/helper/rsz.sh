#!/bin/sh
TESTYPE=$1

# Usage:rsz_vbuff <input_file> <output_file> <input_width> <input_height> <output_width> <output_height>
if [ $TESTYPE = "QVGAtoSXGA" ]; then
  FIN="${TESTSCRIPT}/QVGArsz.yuv"
  FNAME="${TMPBASE}/newSXGA.yuv"
  INSIZE="320 240"
  OUTSIZE="1280 960"
elif [ $TESTYPE = "SXGAtoQVGA" ]; then
  FIN="${TESTSCRIPT}/SXGArsz.yuv"
  FNAME="${TMPBASE}/newQVGA.yuv"
  OUTSIZE="320 240"
  INSIZE="1280 960"
elif [ $TESTYPE = "5MPtoQCIF" ]; then
  FIN="${TESTSCRIPT}/5MPrsz.yuv"
  FNAME="${TMPBASE}/newQCIF.yuv"
  INSIZE="2592 1944"
  OUTSIZE="176 144"
fi
${TESTBIN}/rsz_vbuff $FIN $FNAME $INSIZE $OUTSIZE
sleep 1
chmod 744 $FNAME

if [ -z "$STRESS" ]; then
  echo "";echo "Was resizer capabled of change $FIN with $INSIZE size to $OUTSIZE size into $FNAME file?";echo ""
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

