#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_OPERATION=$1

# =============================================================================
# Functions
# =============================================================================

setOneFrequency() {

	LOCAL_FREQUENCY=$1
	LOCAL_COMMAND_LINE=$2

	LOCAL_LOOP_NUMBER=0

	LOCAL_FREQUENCY_NUMBER=`echo ${LOCAL_FREQUENCY#OPP}`
	LOCAL_FREQUENCIES_LIST_AVAILABLE=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies`

	for FREQUENCY in $LOCAL_FREQUENCIES_LIST_AVAILABLE
	do
		LOCAL_LOOP_NUMBER=`expr $LOCAL_LOOP_NUMBER + 1`
		echo $FREQUENCY	> $HCFSF_FREQUENCIES_LIST_AVAILABILITY.$LOCAL_LOOP_NUMBER
	done

	if [ "$LOCAL_FREQUENCY_NUMBER" -gt "$LOCAL_LOOP_NUMBER" ]; then

		LOCAL_FREQUENCY_NUMBER=$LOCAL_LOOP_NUMBER

	fi

	LOCAL_FREQUENCY=`cat $HCFSF_FREQUENCIES_LIST_AVAILABILITY.$LOCAL_FREQUENCY_NUMBER`

	if [ -n $LOCAL_COMMAND_LINE ]; then
		eval $LOCAL_COMMAND_LINE &
	fi

	echo $LOCAL_FREQUENCY > /sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed
	if [ $? != 0 ]; then
		echo "Info: Error! Frequency $i cannot be set"
	else
		echo "Info: Frequency $LOCAL_FREQUENCY was correctly set"
	fi

	wait

	sleep 5
}

setAllFrequencies() {

	LOCAL_COMMAND_LINE=$1

	error=0
	echo > $HCFSF_FREQUENCIES_LIST_OK
	echo > $HCFSF_FREQUENCIES_LIST_ERROR

	LOCAL_FREQUENCIES_LIST_AVAILABLE=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies`

	if [ -n $LOCAL_COMMAND_LINE ]; then
		$LOCAL_COMMAND_LINE &
		LOCAL_COMMAND_PID=`echo $!`
	fi


	while [ 1 ]; do

		for i in $LOCAL_FREQUENCIES_LIST_AVAILABLE

		do
			echo "Info: Setting Governor to" $i
			echo $i > /sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed
			if [ $? != 0 ]; then
				echo "Info: Error! Frequency $i cannot be set"
				echo $i >> $HCFSF_FREQUENCIES_LIST_ERROR
				error=1
			else
				echo "Info: Governor $i was correctly set"
				echo $i >> $HCFSF_FREQUENCIES_LIST_OK
			fi
			sleep 1
		done

		if [ -n $LOCAL_COMMAND_LINE ]; then
			test -d /proc/$LOCAL_COMMAND_PID || break
		else
			break
		fi

	done

	wait

	echo "Info: The following frequencies were correctly set"
	cat $HCFSF_FREQUENCIES_LIST_OK
	echo "Info: The following frequencies were not correctly set"
	cat $HCFSF_FREQUENCIES_LIST_ERROR

	sleep 5

	if [ $error -eq 1 ]; then
		exit 1
	fi
}

# =============================================================================
# Main
# =============================================================================

handlerError.sh "test"
if [ $? -eq 1 ]; then
	return 1
fi

if [ ! -f /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies ]; then
	echo "FATAL: /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies cannot be found!"
	handlerError.sh "log" "1" "halt" "handlerCpuFreqScalFrequencies.sh"
	exit 1
fi

if [ ! -f /sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed ]; then
	echo "FATAL: /sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed cannot be found!"
	handlerError.sh "log" "1" "halt" "handlerCpuFreqScalFrequencies.sh"
	exit 1
fi

if [ "$LOCAL_OPERATION" = "set" ]; then

	LOCAL_FREQUENCY=$2

	if [ "$LOCAL_FREQUENCY" = "all" ]; then
		setAllFrequencies
	else
		setOneFrequency $LOCAL_FREQUENCY
	fi

elif [ "$LOCAL_OPERATION" = "run" ]; then

	LOCAL_FREQUENCY=$2
	LOCAL_COMMAND_LINE=$3

	if [ "$LOCAL_FREQUENCY" = "all" ]; then
		setAllFrequencies $LOCAL_COMMAND_LINE
	else
		setOneFrequency $LOCAL_FREQUENCY $LOCAL_COMMAND_LINE
	fi

fi

# End of ile
