#!/bin/sh

n=0
i=0
while [ "$i" != "1000" ]
do

n=$RANDOM
n=$(($n%10))

DEVICE="omap-previewer"
DEVICE="-idev /dev/$DEVICE"
OUTFILE="${TMPBASE}/file.yuv"

if [ "$n" = "0" ]; then
   TEST="WBAL"
   TESTPARAM="red"
   INSIZE="-iw 3264 -ih 2464"
   INFILE="-ifile ${TESTSCRIPT}/8MP.raw"
elif [ "$n" = "1" ]; then
   TEST="CFA"
   TESTPARAM="disabled"
   INSIZE="-iw 2592 -ih 1944"
   INFILE="-ifile ${TESTSCRIPT}/5MP.raw"
elif [ "$n" = "2" ]; then
   TEST="BRIGHTNESS"
   TESTPARAM="default"
   INSIZE="-iw 1024 -ih 768"
   INFILE="-ifile ${TESTSCRIPT}/XGA.raw"
elif [ "$n" = "3" ]; then
   TEST="NF"
   TESTPARAM="disabled"
   INSIZE="-iw 640 -ih 480"
   INFILE="-ifile ${TESTSCRIPT}/VGA.raw"
elif [ "$n" = "4" ]; then
   TEST="CONTRAST"
   TESTPARAM="minimum"
   INSIZE="-iw 640 -ih 480"
   INFILE="-ifile ${TESTSCRIPT}/VGA.raw"
elif [ "$n" = "5" ]; then
   TEST="WBAL"
   TESTPARAM="green"
   INSIZE="-iw 640 -ih 480"
   INFILE="-ifile ${TESTSCRIPT}/VGA.raw"
elif [ "$n" = "6" ]; then
   TEST="CFA"
   TESTPARAM="disabled"
   INSIZE="-iw 640 -ih 480"
   INFILE="-ifile ${TESTSCRIPT}/VGA.raw"
elif [ "$n" = "7" ]; then
   TEST="BRIGHTNESS"
   TESTPARAM="maximum"
   INSIZE="-iw 640 -ih 480"
   INFILE="-ifile ${TESTSCRIPT}/VGA.raw"
elif [ "$n" = "8" ]; then
   TEST="NF"
   TESTPARAM="disabled"
   INSIZE="-iw 640 -ih 480"
   INFILE="-ifile ${TESTSCRIPT}/VGA.raw"
elif [ "$n" = "9" ]; then
   TEST="CONTRAST"
   TESTPARAM="maximum"
   INSIZE="-iw 640 -ih 480"
   INFILE="-ifile ${TESTSCRIPT}/VGA.raw"
elif [ "$n" = "10" ]; then
   TEST="WBAL"
   TESTPARAM="blue"
   INSIZE="-iw 640 -ih 480"
   INFILE="-ifile ${TESTSCRIPT}/VGA.raw"
fi

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

i=$(($i+1)) #while loop count increments here

ERR=0

if [ $RESULT -eq 255 ]; then
   ERR=1
   elif [ -z "$STRESS" ]; then

if [ "$TEST" = "INFO" ]; then
    echo "";echo "Was the displayed information for $INDEVICE correct?";echo ""
else
   echo "";echo "Was camera able to convert $INFILE to YUV\
   and save into $OUTFILE?";echo ""
fi
   $WAIT_ANSWER
   ERR=$?
fi


done #while loop ends here

if [ $ERR -eq 1 ]; then
   #color output
   echo -en "\033[0;31m FAIL \033[0m \n"
   exit 1
else
   echo -en "\033[0;32m PASS \033[0m \n"
   exit 0
fi

## test script ends
