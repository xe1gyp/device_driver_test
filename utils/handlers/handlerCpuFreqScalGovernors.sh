#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_OPERATION=$1

# =============================================================================
# Functions
# =============================================================================

setOneGovernor() {

	LOCAL_GOVERNOR=$1
	LOCAL_COMMAND_LINE=$2

	if [ -n $LOCAL_COMMAND_LINE ]; then
		eval $LOCAL_COMMAND_LINE &
	fi

	echo $LOCAL_GOVERNOR > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
	if [ $? != 0 ]; then
		echo "Info: Error! Governor $i cannot be set"
	else
		echo "Info: Governor $LOCAL_GOVERNOR was correctly set"
	fi

	wait

	sleep 5
}

setAllGovernor() {

	LOCAL_COMMAND_LINE=$@

	error=0
	echo > $HCFSG_GOVERNORS_LIST_OK
	echo > $HCFSG_GOVERNORS_LIST_ERROR

	LOCAL_GOVERNORS_LIST_AVAILABLE=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors`
	echo "Info: Available Governors are -> $LOCAL_GOVERNORS_LIST_AVAILABLE"

	if [ -n "$LOCAL_COMMAND_LINE" ]; then
		$LOCAL_COMMAND_LINE &
		LOCAL_COMMAND_PID=`echo $!`
	fi

	while [ 1 ]; do

		for i in $LOCAL_GOVERNORS_LIST_AVAILABLE

		do
			echo "Setting Governor to" $i
			echo $i > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
			if [ $? != 0 ]; then
				echo "Info: Error! Governor $i cannot be set"
				echo $i >> $HCFSG_GOVERNORS_LIST_ERROR
				error=1
			else
				echo "Info: Governor $i was correctly set"
				echo $i >> $HCFSG_GOVERNORS_LIST_OK
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

	echo "Info: The following Governors were correctly set"
	cat $HCFSG_GOVERNORS_LIST_OK
	echo "Info: The following Governors were not correctly set"
	cat $HCFSG_GOVERNORS_LIST_ERROR

	sleep 5

	if [ $error -eq 1 ]; then
		exit 1
	fi
}

getCurrentGovernor() {
	LOCAL_FILE=$1
	LOCAL_GOVERNOR=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor`
	echo "Info: Current Governor -> $LOCAL_GOVERNOR"

	echo $LOCAL_GOVERNOR > $LOCAL_FILE
}

restoreCurrentGovernor() {
	LOCAL_FILE=$1

	if [ -f $LOCAL_FILE ]; then
		cat $LOCAL_FILE > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
		echo "Info: Restore to Governor -> `cat $LOCAL_FILE`"
	else
		echo 'Error: $LOCAL_FILE parameter is empty'
	fi
}

# =============================================================================
# Main
# =============================================================================

handlerError.sh "test"
if [ $? -eq 1 ]; then
	exit 1
fi


if [ ! -f /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors ]; then
	echo "FATAL: /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors cannot be found!"
	handlerError.sh "log" "1" "halt" "handlerCpuFreqScalGovernors.sh"
	exit 1
fi

if [ ! -f /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor ]; then
	echo "FATAL: /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor cannot be found!"
	handlerError.sh "log" "1" "halt" "handlerCpuFreqScalGovernors.sh"
	exit 1
fi

if [ "$LOCAL_OPERATION" = "set" ]; then

	LOCAL_GOVERNOR=$2

	if [ "$LOCAL_GOVERNOR" = "all" ]; then
		setAllGovernor
	else
		setOneGovernor $LOCAL_GOVERNOR
	fi

elif [ "$LOCAL_OPERATION" = "run" ]; then

	LOCAL_GOVERNOR=$2
	LOCAL_COMMAND_LINE=$3

	if [ "$LOCAL_GOVERNOR" = "all" ]; then
		setAllGovernor "$LOCAL_COMMAND_LINE"
	else
		setOneGovernor $LOCAL_GOVERNOR "$LOCAL_COMMAND_LINE"
	fi

elif  [ "$LOCAL_OPERATION" = "get" ]; then

	getCurrentGovernor $HCFSG_CURRENT_GOVERNOR_FILE

elif  [ "$LOCAL_OPERATION" = "restore" ]; then

	restoreCurrentGovernor $HCFSG_CURRENT_GOVERNOR_FILE

fi

# End of file
