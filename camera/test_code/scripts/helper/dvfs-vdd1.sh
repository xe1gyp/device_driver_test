#!/bin/sh

DEVICE=$1
FPS=$2
FORMAT=$3
SIZE=$4


# Run streaming in background
if [ "$SIZE" = "VGA" ]; then
time $TESTBIN/streaming_frame $DEVICE $FPS $FORMAT $SIZE &
TEST="streaming_frame"
else
time $TESTBIN/fps $DEVICE $FORMAT $SIZE 250 $FPS &
TEST="fps"
fi

sleep 1

# Find pid for the background test.
pid=`ps | grep $TEST | grep -v "grep" | awk '{print $1}'`

# Changing the governor to userspace.
GOV=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor`
echo "userspace" > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor

# Checking if scaling_setspeed has been created
if [ ! -e /sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed ]
then
	echo "Fatal: scaling_setspeed is not available"
	exit 1
fi

# Changing through all the frequencies available
available_frequencies=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies`

for i in $available_frequencies
do
	sleep 5
	echo ""
	echo "Setting ARM Frequency to" $i "KHz"
	echo ""
	echo $i > /sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed
	cur_freq=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq`
	if [ "$i" != "$cur_freq" ]; then
		echo "ERROR: Current frequency $cur_freq KHz is different from requested one!"
	fi
done
sleep 5

# Wait for background process to finish.
while [ "$pid" != "" ]
do
	pid=`ps | grep $TEST | grep -v "grep" | awk '{print $1}'`
	sleep 1
done


# Switch back to original governor.
echo $GOV > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
echo "Reset Governor to:" `cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor`


if [ -z "$STRESS" ]; then
    echo "";echo " Was streaming video at $FPS fps and OPP values displayed cleanly on LCD screen?";echo ""
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
