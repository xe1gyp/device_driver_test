#!/bin/sh
FOLDER_ENTRIES=/sys/devices/system/cpu/cpu0/cpufreq/
SETSPEED_ENTRY=$FOLDER_ENTRIES/scaling_setspeed
GOVERNOR_ENTRY=$FOLDER_ENTRIES/scaling_governor
FREQUENCIES_ENTRY=$FOLDER_ENTRIES/scaling_available_frequencies
CUR_FREQUENCY_ENTRY=$FOLDER_ENTRIES/scaling_cur_freq

# Changing the governor to userspace
echo "userspace" > $GOVERNOR_ENTRY

# Checking if scaling_setspeed has been created
if [ ! -e $SETSPEED_ENTRY ]
then
	echo "Fatal: scaling_setspeed is not available"
	exit 1
fi

# Changing through all the frequencies available
available_frequencies=`cat $FREQUENCIES_ENTRY`

while true
do
	for i in $available_frequencies
	do
		echo "Setting ARM Frequency to" $i
		echo $i > $SETSPEED_ENTRY
		cur_frequency=`cat $CUR_FREQUENCY_ENTRY`
		if [ "$i" != "$cur_frequency" ]
		then
			echo "Current frequency is different from the set one"
		fi
	done
done
