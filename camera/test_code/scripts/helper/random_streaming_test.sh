#!/bin/sh

i=0
while [ "$i" != "1000" ]
do

n=$RANDOM
n=$(($n%10))

FORMAT="YUYV"
DEVICE=5

if [ "$n" = "0" ]; then
   SIZE="128 96" #SQCIF
   FPS=15
elif [ "$n" = "1" ]; then
   SIZE="320 240" #QVGA
   FPS=15
elif [ "$n" = "2" ]; then
   SIZE="1280 720" #720p
   FPS=30
elif [ "$n" = "3" ]; then
   SIZE="800 480" #WVGA
   FPS=15
elif [ "$n" = "4" ]; then
   SIZE="640 480" #VGA
   FPS=15
elif [ "$n" = "5" ]; then
   SIZE="176 144" #QCIF
   FPS=15
elif [ "$n" = "6" ]; then
   SIZE="352 288" #CIF
   FPS=15
elif [ "$n" = "7" ]; then
   SIZE="720 486" #D1NTSC
   FPS=30
elif [ "$n" = "8" ]; then
   SIZE="720 576" #D1PAL
   FPS=30
elif [ "$n" = "9" ]; then
   SIZE="160 120" #QQVGA
   FPS=30
elif [ "$n" = "10" ]; then
   SIZE="88 72" #QQCIF
   FPS=30
fi

echo 1 > /sys/class/graphics/fb0/blank # turn off display
echo 0 > /sys/class/graphics/fb0/blank # keep display awake
$TESTBIN/streaming_frame $DEVICE $FPS $FORMAT $SIZE
RESULT=$?
echo "Test returned $RESULT"

i=$(($i+1)) #while loop count increments here

if [ $RESULT -eq 255 ]; then
   ERR=1
elif [ -z "$STRESS" ]; then
   echo "";echo " Was streaming $FORMAT video at $FPS fps in $SIZE size displayed cleanly on LCD screen?";echo ""
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
