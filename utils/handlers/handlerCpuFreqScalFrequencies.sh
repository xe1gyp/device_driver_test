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
	LOCAL_FREQUENCIES_LIST_AVAILABLE=`cat $SYSFS_CPU0_AVAILABLE_FREQUENCIES`

	for FREQUENCY in $LOCAL_FREQUENCIES_LIST_AVAILABLE
	do
		LOCAL_LOOP_NUMBER=`expr $LOCAL_LOOP_NUMBER + 1`
		echo $FREQUENCY	> $HCFSF_FREQUENCIES_LIST_AVAILABILITY.$LOCAL_LOOP_NUMBER
	done

	if [ "$LOCAL_FREQUENCY_NUMBER" -gt "$LOCAL_LOOP_NUMBER" ]; then

		LOCAL_FREQUENCY_NUMBER=$LOCAL_LOOP_NUMBER

	fi

	LOCAL_FREQUENCY=`cat $HCFSF_FREQUENCIES_LIST_AVAILABILITY.$LOCAL_FREQUENCY_NUMBER`

	if [ -n "$LOCAL_COMMAND_LINE" ]; then
		eval $LOCAL_COMMAND_LINE &
	fi

	echo $LOCAL_FREQUENCY > $SYSFS_CPU0_SET_SPEED
	LOCAL_CUR_FREQ=`cat $SYSFS_CPU0_CURRENT_FREQUENCY`

	if [ $LOCAL_FREQUENCY -ne $LOCAL_CUR_FREQ ]; then
		echo "Info: Error! Frequency $i coudl not be set"
	else
		echo "Info: Frequency $LOCAL_FREQUENCY was correctly set"
	fi

	wait

	sleep 5
}

setAllFrequencies() {

	LOCAL_COMMAND_LINE=$@

	error=0
	echo > $HCFSF_FREQUENCIES_LIST_OK
	echo > $HCFSF_FREQUENCIES_LIST_ERROR

	LOCAL_FREQUENCIES_LIST_AVAILABLE=`cat $SYSFS_CPU0_AVAILABLE_FREQUENCIES`
	echo "Info: Available frequencies are -> `echo $LOCAL_FREQUENCIES_LIST_AVAILABLE`"

	if [ -n "$LOCAL_COMMAND_LINE" ]; then
		$LOCAL_COMMAND_LINE &
		LOCAL_COMMAND_PID=`echo $!`
	fi

	while [ 1 ]; do

		for i in $LOCAL_FREQUENCIES_LIST_AVAILABLE
		do
			echo "Info: Setting Frequency to" $i
			echo $i > $SYSFS_CPU0_SET_SPEED
			LOCAL_CUR_FREQ=`cat $SYSFS_CPU0_CURRENT_FREQUENCY`

			if [ $i -ne $LOCAL_CUR_FREQ ]; then
				echo "Info: Error! Frequency $i cannot be set"
				echo $i >> $HCFSF_FREQUENCIES_LIST_ERROR
				error=1
			else
				echo "Info: Frequency $i was correctly set"
				echo $i >> $HCFSF_FREQUENCIES_LIST_OK
			fi
			sleep 1
		done

		if [ -n "$LOCAL_COMMAND_LINE" ]; then
			test -d /proc/$LOCAL_COMMAND_PID || break
		else
			break
		fi

	done

	wait

  echo
	echo "Info: The following frequencies were correctly set"
	cat $HCFSF_FREQUENCIES_LIST_OK
	echo
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
	exit 1
fi

if [ ! -f $SYSFS_CPU0_AVAILABLE_FREQUENCIES ]; then
	echo "FATAL: $SYSFS_CPU0_AVAILABLE_FREQUENCIES cannot be found!"
	handlerError.sh "log" "1" "halt" "handlerCpuFreqScalFrequencies.sh"
	exit 1
fi

if [ ! -f $SYSFS_CPU0_SET_SPEED ]; then
	echo "FATAL: $SYSFS_CPU0_SET_SPEED cannot be found!"
	handlerError.sh "log" "1" "halt" "handlerCpuFreqScalFrequencies.sh"
	exit 1
fi

handlerCpuFreqScalGovernors.sh "get"
handlerCpuFreqScalGovernors.sh "set" "userspace"

if [ "$LOCAL_OPERATION" = "list" ]; then

	cat $SYSFS_CPU0_AVAILABLE_FREQUENCIES

elif [ "$LOCAL_OPERATION" = "set" ]; then

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
		setAllFrequencies "$LOCAL_COMMAND_LINE"
	else
		setOneFrequency $LOCAL_FREQUENCY "$LOCAL_COMMAND_LINE"
	fi

elif [ "$LOCAL_OPERATION" = "set_fail" ]; then

	# Changing through unexisting frequencies
	available_frequencies=" 123456 654321 123654 456123"
	for i in $available_frequencies
	do
		echo "Setting Frequency to " $i
		echo "echo $i > $SYSFS_CPU0_SET_SPEED"
		echo $i > $SYSFS_CPU0_SET_SPEED
		cur_frequency=`cat $SYSFS_CPU0_CURRENT_FREQUENCY`
		if [ "$i" = "$cur_frequency" ]
		then
			echo "Fatal: Frequency was changed, unexpected!"
			LOCAL_ERROR=1
		else
			echo "Info: Frequency was not changed, good!"
			LOCAL_ERROR=0
		fi
  done

fi

handlerCpuFreqScalGovernors.sh "restore"

exit $LOCAL_ERROR

# End of file
