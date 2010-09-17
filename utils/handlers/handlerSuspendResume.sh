#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_OPERATION=$1
LOCAL_WAKEUP_TIME=$2

# =============================================================================
# Functions
# =============================================================================

suspendResume() {

	LOCAL_WAKEUP_TIME=$1
	LOCAL_COMMAND_LINE=$2
	LOCAL_ERROR=0

	if [ -n "$LOCAL_COMMAND_LINE" ]; then
		$LOCAL_COMMAND_LINE &
		LOCAL_COMMAND_PID=`echo $!`
	fi

	echo $LOCAL_WAKEUP_TIME > $PM_WAKEUP_TIMER_SECONDS

	while [ 1 ]; do

		echo > /var/log/messages
		sleep 1
		echo mem > /sys/power/state

		cat /var/log/messages | grep "$HSR_SUSPEND_RESUME_MESSAGE_SUCCESS"
		if [ $? != 0 ]; then
			echo "Error: all powerdomains did not enter target state"
			cat /var/log/messages
			LOCAL_ERROR=1
		fi

		if [ -n "$LOCAL_COMMAND_LINE" ]; then
			test -d /proc/$LOCAL_COMMAND_PID || break
		else
			break
		fi

	done

	wait
	exit $LOCAL_ERROR
}

# =============================================================================
# Main
# =============================================================================

handlerError.sh "test"
if [ $? -eq 1 ]; then
	exit 1
fi

if [ ! -f "$PM_WAKEUP_TIMER_SECONDS" ]; then
	echo "FATAL: $PM_WAKEUP_TIMER_SECONDS cannot be found!"
	handlerError.sh "log" "1" "halt" "handlerSuspendResume"
	exit 1
fi

if [ ! -f /sys/power/state ]; then
	echo "FATAL: /sys/power/state cannot be found!"
	handlerError.sh "log" "1" "halt" "handlerSuspendResume"
	exit 1
fi

handlerDebugFileSystem.sh "mount"

if [ "$LOCAL_OPERATION" = "suspend" ]; then
	suspendResume $LOCAL_WAKEUP_TIME
elif [ "$LOCAL_OPERATION" = "run" ]; then
	LOCAL_COMMAND_LINE=$3
	suspendResume $LOCAL_WAKEUP_TIME "$LOCAL_COMMAND_LINE"
fi

handlerDebugFileSystem.sh "umount"

# End of file
