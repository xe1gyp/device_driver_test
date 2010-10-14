#!/bin/sh
# Changing the governor to userspace 
echo "userspace" > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
 
# Checking if scaling_setspeed has been created
if [ ! -e /sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed ]
then
	echo "Fatal: scaling_setspeed is not available"
	exit 1
fi

# Changing through all the frequencies available
available_frequencies=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies`

while true 
 do
	for i in $available_frequencies
	do
		echo "Setting ARM Frequency to" $i
		echo $i > /sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed
		cur_frequency=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq`
		if [ "$i" != "$cur_frequency" ]
		then
			echo "Fatal: Current frequency is different from the set one"
			exit 1
		fi
	done
 done

