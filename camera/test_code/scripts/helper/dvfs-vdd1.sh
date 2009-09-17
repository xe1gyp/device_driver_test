#!/bin/sh

DEVICE=$1
FPS=$2
FORMAT=$3
SIZE=$4

MINCOUNT=0
vdd1_opp_no=5

echo -n $vdd1_opp_no > /sys/power/vdd1_lock

#Run streaming at background
if [ "$SIZE" = "VGA" ]; then
time $TESTBIN/streaming_frame $DEVICE $FPS $FORMAT $SIZE &
else
time $TESTBIN/fps $DEVICE $FORMAT $SIZE 250 $FPS &
fi


while [ "$vdd1_opp_no" -ne $MINCOUNT ]
do
	sleep 5
	echo -n $vdd1_opp_no > /sys/power/vdd1_lock
	echo VDD1:
	cat /sys/power/vdd1_opp
	vdd1_opp_no=`expr $vdd1_opp_no - 1`
done
sleep 5

if [ -z "$STRESS" ]; then
    echo "";echo " Was streaming video at $FPS fbs and OPP values displayed cleanly on LCD screen?";echo ""
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
