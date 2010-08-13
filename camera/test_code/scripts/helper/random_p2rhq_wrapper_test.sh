#!/bin/sh

n=0
i=0
while [ "$i" != "1000" ]
do

n=$RANDOM
n=$(($n%10))

FNAME="${TMPBASE}/file.yuv"

if [ "$n" = "0" ]; then
   TESTYPE="8MPto5MP"
   FIN="${TESTSCRIPT}/8MP.raw"
   INSIZE="-iw 3280 -ih 2464"
   OUTSIZE="-ow 2592 -oh 1944"
elif [ "$n" = "1" ]; then
   TESTYPE="5MPtoQXGA"
   FIN="${TESTSCRIPT}/5MP.raw"
   INSIZE="-iw 2592 -ih 1944"
   OUTSIZE="-ow 2048 -oh 1536"
elif [ "$n" = "2" ]; then
   TESTYPE="XGAtoSVGA"
   FIN="${TESTSCRIPT}/XGA.raw"
   INSIZE="-iw 1024 -ih 768"
   OUTSIZE="-ow 800 -oh 600"
elif [ "$n" = "3" ]; then
   TESTYPE="VGAtoQVGA"
   FIN="${TESTSCRIPT}/VGA.raw"
   INSIZE="-iw 640  -ih 480"
   OUTSIZE="-ow 320 -oh 240"
elif [ "$n" = "4" ]; then
   TESTYPE="VGAtoQXGA"
   FIN="${TESTSCRIPT}/VGA.raw"
   INSIZE="-iw 640  -ih 480"
   OUTSIZE="-ow 2048 -oh 1536"
elif [ "$n" = "5" ]; then
   TESTYPE="XGAto5MP"
   FIN="${TESTSCRIPT}/XGA.raw"
   INSIZE="-iw 1024 -ih 768"
   OUTSIZE="-ow 2592 -oh 1944"
elif [ "$n" = "6" ]; then
   TESTYPE="5MPto8MP"
   FIN="${TESTSCRIPT}/5MP.raw"
   INSIZE="-iw 2592 -ih 1944"
   OUTSIZE="-ow 3280 -oh 2464"
elif [ "$n" = "7" ]; then
   TESTYPE="5MPtoUXGA"
   FIN="${TESTSCRIPT}/5MP.raw"
   INSIZE="-iw 2592 -ih 1944"
   OUTSIZE="-ow 1600 -oh 1200"
elif [ "$n" = "8" ]; then
   TESTYPE="XGAtoVGA"
   FIN="${TESTSCRIPT}/XGA.raw"
   INSIZE="-iw 1024 -ih 768"
   OUTSIZE="-ow 640 -oh 480"
elif [ "$n" = "9" ]; then
   TESTYPE="VGAtoQCIF"
   FIN="${TESTSCRIPT}/VGA.raw"
   INSIZE="-iw 640  -ih 480"
   OUTSIZE="-ow 176 -oh 144"
elif [ "$n" = "10" ]; then
   TESTYPE="VGAtoSXGA"
   FIN="${TESTSCRIPT}/VGA.raw"
   INSIZE="-iw i640 -ih 480"
   OUTSIZE="-ow 1280 -oh 960"
fi

${TESTBIN}/p2rhq -ifile $FIN -ofile $FNAME $INSIZE $OUTSIZE
RESULT=$?
echo "Test returned $RESULT"
sleep 1
chmod 744 $FNAME

i=$(($i+1)) #while loop count increments here

if [ $? -eq 1 ]; then
  #color output - The output file wasn't created
  echo -en "Failed to chmod the $FNAME \n"
  echo -en "\033[0;31m FAIL \033[0m \n"
  exit 1
fi

#initialize ERR var with PASS
ERR=0

if [ $RESULT -eq 255 ]; then
   ERR=1
elif [ -z "$STRESS" ]; then
   echo "";echo "Was resizer capable of changing $FIN \
   with $INSIZE size to $OUTSIZE size into $FNAME file?";echo ""
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
