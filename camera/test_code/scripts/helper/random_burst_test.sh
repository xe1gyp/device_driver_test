#!/bin/sh

i=0
while [ "$i" != "1000" ]
do

n=$RANDOM
n=$(($n%10))

FORMAT="YUYV"
DEVICE=5
FRNUM=1
FNAME="${TMPBASE}/file.yuv"

if [ "$n" = "0" ]; then
   SIZE="640 480" #VGA
   FPS=10
elif [ "$n" = "1" ]; then
   SIZE="3280 2464" #8MP
   FPS=10
elif [ "$n" = "2" ]; then
   SIZE="1280 720" #720p
   FPS=30
elif [ "$n" = "3" ]; then
   SIZE="1280 1024" #SXGA
   FPS=10
elif [ "$n" = "4" ]; then
   SIZE="720 486" #D1NTSC
   FPS=10
elif [ "$n" = "5" ]; then
   SIZE="720 576" #D1PAL
   FPS=10
elif [ "$n" = "6" ]; then
   SIZE="800 600" #SVGA
   FPS=10
elif [ "$n" = "7" ]; then
   SIZE="2048 1536" #QXGA
   FPS=10
elif [ "$n" = "8" ]; then
   SIZE="1600 1200" #UXGA
   FPS=10
elif [ "$n" = "9" ]; then
   SIZE="1024 768" #XGA
   FPS=10
elif [ "$n" = "10" ]; then
   SIZE="2592x1944" #5MP
   FPS=10
fi

$TESTBIN/burst_mode $DEVICE $FORMAT $SIZE $FRNUM "$FNAME" $FPS
RESULT=$?
echo "Test returned $RESULT"
chmod 744 $FNAME

i=$(($i+1)) #while loop count increments here

if [ $RESULT -eq 255 ]; then
   ERR=1
elif [ -z "$STRESS" ]; then
   echo "";echo "Was camera able to capture $FRNUM frame(s) in $FORMAT format of $SIZE size and saved into $FNAME?";echo ""
   $WAIT_ANSWER
   ERR=$?
fi

done  #while loop ends here

if [ $ERR -eq 1 ]; then
   echo "FAIL"
   exit 1
else
   echo "PASS"
   exit 0
fi
