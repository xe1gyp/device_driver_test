#!/bin/sh

n=0
i=0
while [ "$i" != "1000" ]
do

n=$RANDOM
n=$(($n%10))

FNAME="${TMPBASE}/file.yuv"

if [ "$n" = "0" ]; then
   TESTYPE="8MPtoXGA"
   FIN="${TESTSCRIPT}/8MPrsz.yuv"
   INSIZE="-iw 3280 -ih 2464"
   OUTSIZE="-ow 1024 -oh 768"
elif [ "$n" = "1" ]; then
   TESTYPE="UXGAtoVGA"
   FIN="${TESTSCRIPT}/UXGArsz.yuv"
   INSIZE="-iw 1600 -ih 1200"
   OUTSIZE="-ow 640 -oh 480"
elif [ "$n" = "2" ]; then
   TESTYPE="SVGAtoQVGA"
   FIN="${TESTSCRIPT}/SVGArsz.yuv"
   INSIZE="-iw 800 -ih 600"
   OUTSIZE="-ow 320 -oh 240"
elif [ "$n" = "3" ]; then
   TESTYPE="QQVGAtoSQCIF"
   FIN="${TESTSCRIPT}/QQVGArsz.yuv"
   INSIZE="-iw 160 -ih 120"
   OUTSIZE="-ow 128 -oh 96"
elif [ "$n" = "4" ]; then
   TESTYPE="SXGAto8MP"
   FIN="${TESTSCRIPT}/SXGArsz.yuv"
   INSIZE="-iw 1280 -ih 960"
   OUTSIZE="-ow 3280 -oh 2464"
elif [ "$n" = "5" ]; then
   TESTYPE="XGAto5MP"
   FIN="${TESTSCRIPT}/XGArsz.yuv"
   INSIZE="-iw 1024 -ih 768"
   OUTSIZE="-ow 2592 -oh 1944"
elif [ "$n" = "6" ]; then
   TESTYPE="VGAtoQXGA"
   FIN="${TESTSCRIPT}/VGArsz.yuv"
   INSIZE="-iw 640  -ih 480"
   OUTSIZE="-ow 2048 -oh 1536"
elif [ "$n" = "7" ]; then
   TESTYPE="SQCIFtoCIF"
   FIN="${TESTSCRIPT}/SQCIFrsz.yuv"
   INSIZE="-iw 128 -ih 96"
   OUTSIZE="-ow 352 -oh 288"
elif [ "$n" = "8" ]; then
   TESTYPE="QVGAtoVGA"
   FIN="${TESTSCRIPT}/QVGArsz.yuv"
   INSIZE="-iw 320  -ih 240"
   OUTSIZE="-ow 640 -oh 480"
elif [ "$n" = "9" ]; then
   TESTYPE="VGAtoQQVGA"
   FIN="${TESTSCRIPT}/VGArsz.yuv"
   INSIZE="-iw 640  -ih 480"
   OUTSIZE="-ow 160 -oh 120"
elif [ "$n" = "10" ]; then
   TESTYPE="5MPto8MP"
   FIN="${TESTSCRIPT}/5MPrsz.yuv"
   INSIZE="-iw 2592 -ih 1944"
   OUTSIZE="-ow 3280 -oh 2464"
fi

${TESTBIN}/reszhq -ifile $FIN -ofile $FNAME $INSIZE $OUTSIZE
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
