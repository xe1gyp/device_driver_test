#!/bin/sh

ISIZE=$1
OSIZE=$2
OFORMAT=$3
IDEVICE=$4
ODEVICE=$5
TEST=$6
TESTPARAM=$7

if [ "$ISIZE" = "8MP" ]; then
  INSIZE="-iw 3280 -ih 2464"
  in_sz="3280x2464"
fi

if [ "$OSIZE" = "8MP" ]; then
  OUTSIZE="-ow 3280 -oh 2464"
  out_sz="3280x2464"
fi

if [ "$ISIZE" = "5MP" ]; then
  INSIZE="-iw 2592 -ih 1944"
  in_sz="2592x1944"
fi

if [ "$OSIZE" = "5MP" ]; then
  OUTSIZE="-ow 2592 -oh 1944"
  out_sz="2592x1944"
fi

if [ "$ISIZE" = "3264x2464" ]; then
  INFILE="${TESTSCRIPT}/8MP.raw"
  INSIZE="-iw 3264 -ih 2464"
  in_sz="3264x2464"
fi

if [ "$OSIZE" = "3264x2464" ]; then
  OUTSIZE="-ow 3264 -oh 2464"
  out_sz="3264x2464"
fi

if [ "$OFORMAT" = "RAW" ]; then
OUTFILE="${TMPBASE}/virtsenhq_out_${TEST}-${TESTPARAM}-${ISIZE}"to"${OSIZE}"_"${out_sz}"."${OFORMAT}"
elif [ "$OFORMAT" = "YUYV" ]; then
OUTFILE="${TMPBASE}/virtsenhq_out_${TEST}-${TESTPARAM}-${ISIZE}"to"${OSIZE}"_"${out_sz}"_YUY2.yuv
elif [ "$OFORMAT" = "UYVY" ]; then
OUTFILE="${TMPBASE}/virtsenhq_out_${TEST}-${TESTPARAM}-${ISIZE}"to"${OSIZE}"_"${out_sz}"_"${OFORMAT}".yuv
   if [ "$TEST" = "BRIGHTNESS" -o "$TEST" = "CONTRAST" ]; then
      if [ "$TESTPARAM" = "maximum" ]; then
          OUTFILE="${TMPBASE}/virtsenhq_out_${TEST}"_M"-${ISIZE}"to"${OSIZE}"_"${out_sz}"_"${OFORMAT}".yuv
      fi
   fi
fi

INDEVICE="-idev /dev/video$IDEVICE"
OUTDEVICE="-odev /dev/video$ODEVICE"

if [ -z "$6" ]; then
  # Usage: virtsenhq ?
  $TESTBIN/virtsenhq $INSIZE $OUTSIZE -of $OFORMAT $INDEVICE $OUTDEVICE -ofile $OUTFILE
  RESULT=$?
  echo " Test returned $RESULT"
elif [ "$TEST" = "ZOOM" ]; then
   # Usage: virtsenhq ?
   $TESTBIN/virtsenhq $INSIZE $OUTSIZE -of $OFORMAT $INDEVICE $OUTDEVICE -ofile $OUTFILE -zoom $TESTPARAM
   RESULT=$?
   echo "Test returned $RESULT"
elif [ "$TEST" = "BRIGHTNESS" ]; then
   # Usage: virtsenhq ?
   $TESTBIN/virtsenhq $INSIZE $OUTSIZE -of $OFORMAT $INDEVICE $OUTDEVICE -ofile $OUTFILE -brightness $TESTPARAM
   RESULT=$?
   echo "Test returned $RESULT"
elif [ "$TEST" = "CONTRAST" ]; then
   # Usage: virtsenhq ?
   $TESTBIN/virtsenhq $INSIZE $OUTSIZE -of $OFORMAT $INDEVICE $OUTDEVICE -ofile $OUTFILE -contrast $TESTPARAM
   RESULT=$?
   echo "Test returned $RESULT"
elif [ "$TEST" = "COLOR" ]; then
    # Usage: busrt_mode <camDevice><pixelFmt><size><num_of_images><file_name><effect>
   $TESTBIN/virtsenhq $INSIZE $OUTSIZE -of $OFORMAT $INDEVICE $OUTDEVICE -ofile $OUTFILE -color $TESTPARAM
   RESULT=$?
   echo "Test returned $RESULT"
elif [ "$TEST" = "INFO" ]; then
   # Usage: virtsenhq ?
   $TESTBIN/virtsenhq $INDEVICE -outputinfo $TESTPARAM
   RESULT=$?
   echo "Test returned $RESULT"
elif [ "$TEST" = "H3A" ]; then
   # Usage: virtsenhq ?
   $TESTBIN/virtsenhq $INSIZE $OUTSIZE -of $OFORMAT $INDEVICE $OUTDEVICE \
   -ofile $OUTFILE -h3a $TESTPARAM
   RESULT=$?
   echo "Test returned $RESULT"
elif [ "$TEST" = "CSI2MEM" ]; then
   # Usage: virtsenhq ?
   $TESTBIN/virtsenhq $INSIZE $OUTSIZE -of $OFORMAT $INDEVICE $OUTDEVICE \
   -ofile $OUTFILE -csi2mem
   RESULT=$?
   echo "Test returned $RESULT"
elif [ "$TEST" = "MEM2CCP" ]; then
   # Usage: virtsenhq ?
   $TESTBIN/virtsenhq $INSIZE $OUTSIZE -of $OFORMAT $INDEVICE $OUTDEVICE \
   -ofile $OUTFILE -ifile $INFILE -mem2ccp
   RESULT=$?
   echo "Test returned $RESULT"
fi

if [ "$TEST" = "INFO" ]; then
echo "Info test doesn't produce output file ... just info."
else
chmod 744 $OUTFILE
fi

ERR=0

if [ $RESULT -eq 255 ]; then
  ERR=1
elif [ -z "$STRESS" ]; then

if [ "$TEST" = "INFO" ]; then
  echo "";echo "Was the displayed information for $INDEVICE correct?";echo ""
else
  echo "";echo "Was camera able to capture $OSIZE in $OFORMAT format and save into $OUTFILE?";echo ""
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

