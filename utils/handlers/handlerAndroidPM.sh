#!/bin/bash

#
# Android Suspend and Resume Handler
# @author: Leed Aguilar
#

#
# =============================================================================
# Variables
# =============================================================================

LOCAL_OPERATION=$1
LOCAL_ENVIRONMENT_TYPE=$2
LOCAL_SUSPEND_PATH=$3
LOCAL_WAKEUP_TIMER=$4
LOCAL_ERROR=0
LOCAL_TOTAL_PARAMETERS=$#
LOCAL_WAKELOCK='android_test_wakelock'
LOCAL_SYSTEM_WAKELOCKS=`cat $SYSTEM_WAKE_LOCK`

# =============================================================================
# Functions
# =============================================================================

# Simulate the END_CALL key press action
# @ Function  : androidPowerKey
# @ Parameters: None
# @ Return    : None
androidPowerKey() {
	showInfo "\tPRESSING POWER KEY"
	# Simulate press END_CALL key
	# OMAP4/3 Scancode for END_CALL key is 62
	sendevent /dev/input/event0 1  62 0
	sendevent /dev/input/event0 1  62 1
	sendevent /dev/input/event0 1  62 0
}

# Simulate the Menu key (screen unlock) press action
# @ Function  : androidMenuKey
# @ Parameters: None
# @ Return    : None
androidMenukey() {
	showInfo "\tPRESSING MENU KEY" \
		 "unlocking Android User Interface"
	# Simulate press Menu key (F1)
	# OMAP4/3 Scancode for F1 key is 59
	sendevent /dev/input/event0 1  59 0
	sendevent /dev/input/event0 1  59 1
}

# This function release all the wakelocks
# given in the wakelock list
# @ Function  : releaseWakelocks
# @ Parameters: <wakelock list>
# @ Return    : None
releaseWakelocks() {
	wakelock_list=( "$@" )
	# Release all the wakelocks under /sys/power/wake_lock
	echo -e "\n----------------------- ANDROID WAKELOCKS --------------------\n"
	for index in ${!wakelock_list[@]}; do
		echo ${wakelock_list[$index]} > $SYSTEM_WAKE_UNLOCK
		if [ `cat $SYSTEM_WAKE_LOCK | grep -wc ${wakelock_list[$index]}` -gt 0 ]; then
			echo -e "\n WAKELOCK: FAIL: < ${wakelock_list[$wakelock]} > was NOT released\n"
		else
			echo -e "\n WAKELOCK: SUCCESS: < ${wakelock_list[$wakelock]} > was released\n"
		fi
	done
	echo -e "\n--------------------------------------------------------------\n"
}

# Hold a wakelock to keep the system awake
# @ Function  : holdWakelock
# @ Parameters: <wakelock name>
# @ Return    : Error flag value
holdWakelock() {
	wakelock_name=$1
	echo $wakelock_name > $SYSTEM_WAKE_LOCK
	# Verify that the wakelock is registered
	if [ `cat $SYSTEM_WAKE_LOCK | grep -wc $wakelock_name` -gt 0 ]; then
		showInfo "ANDROID: Wakelock $LOCAL_WAKELOCK was sucessfully registered"
	else
		showInfo "ERROR: ANDROID: Wakelock $LOCAL_WALELOCK"  "was NOT registered"
		LOCAL_ERROR=1
	fi
}

# Set the System Wakeup timer
# @ Function  : wakeupTimer
# @ Parameters: <timer value in seconds>
# @ Return    : Error flag value
wakeupTimer() {
	timer_value=$1
	handlerSysFs.sh set $PM_WAKEUP_TIMER_SECONDS "$timer_value"
	handlerSysFs.sh compare $PM_WAKEUP_TIMER_SECONDS "$timer_value"
	if [ $? = 1 ]; then
		showInfo "ERROR: wakeup timer was NOT registered"
		LOCAL_ERROR=1
	else
		showInfo "SUCCESS: wakeup timer of $LOCAL_WAKEUP_TIMER seconds was registered"
	fi
}

# Display the script usage
# @ Function  : generalUsage
# @ parameters: None
# @ Return    : Error flag value
generalUsage() {
	cat <<-EOF >&1

	################################################################"

	SCRIPT USAGE:

	    To perform a suspend task do the following:

	    Try - handlerAndroidPM.sh suspend {android|kernel}
	                                      {force|idle}
	                                      {timer}"
	        Where:
	        - android = perform Android early suspend
	        - kernel  = execute kernel global suspend
	        - force   = perform a suspend through suspend path
	        - idle    = perform a suspend through idle path
	        - timer   = how long you want to keep the system is
	                    suspend state
	    To perform a resume task do the following:

	    Try - handlerAndroidPM.sh resume {android|kernel}

	        Where:
	        - android = perform Android late resume
	        - kernel  = wakeup the system till kernel prompt

	###############################################################"

	EOF
	LOCAL_ERROR=1
}

# Prints a message with a specific format
# @ Function  : showInfo
# @ Parameters: <message to display>
# @ Return    : None
showInfo() {
	echo -e "\n\n--------------------------------------------------------------\n"
	messages=( "$@" )
	for index in ${!messages[@]}; do
		echo -e "\t${messages[$index]}"
	done
	echo -e "\n--------------------------------------------------------------\n\n"
}

# Verify LOCAL_ERROR flag
# if flag is set to '1' exit the script and register the failure
# The message parameter helps to debug the script
# @ Function  : verifyErrorFlag
# @ Parameters: <debug message>
# @ Return    : None
verifyErrorFlag() {
	debug_message=$1
	if [ $LOCAL_ERROR -eq 1 ]; then
		handlerError.sh "log" "1" "halt" "handlerAndroidPM.sh"
		handlerDebugFileSystem.sh "umount"
		showInfo "DEBUG: LOCAL ERROR DETECTED:" "$debug_message"  1>&2
		exit $LOCAL_ERROR
	fi
}

# Verify is the parameter is a valid number (integer)
# @ Function  : isPositiveInteger
# @ Parameters: <number>
# @ Return    : Error flag value
isPositiveInteger() {
	num=$1
	if ! [[ $num =~ ^[0-9]+$ ]]; then
		showInfo "ERROR: $num is not a number" 1>&2
		LOCAL_ERROR=1
	fi
}

# =============================================================================
# Main
# =============================================================================

handlerError.sh "test"
if [ $? -eq 1 ]; then
	exit 1
fi

handlerDebugFileSystem.sh "mount"

# Check Parameters and scritp usage

case $LOCAL_OPERATION in
"suspend")
	# Check 2nd parameter
	if [ `echo "android kernel" | grep -wc "$LOCAL_ENVIRONMENT_TYPE"` -ne 1 ]; then
		generalUsage
		verifyErrorFlag "generalUsage(): Check 2nd parameter"
	fi
	# Check 3rd parameter
	if [ `echo "force idle" | grep -wc "$LOCAL_SUSPEND_PATH"` -ne 1 ]; then
		generalUsage
		verifyErrorFlag "generalUsage(): Check 3rd parameter"
	fi
	# Check 4th parameter
	isPositiveInteger $LOCAL_WAKEUP_TIMER
	verifyErrorFlag "generalUsage(): Check 4th parameter"
	if [ $LOCAL_TOTAL_PARAMETERS -ne 4 ]; then
		generalUsage
		verifyErrorFlag "generalUsage(): verify number of parameters (4)"
	fi
	;;
"resume")
	# Check 2nd parameter
	if [ `echo "android kernel" | grep -wc "$LOCAL_ENVIRONMENT_TYPE"` -ne 1 ]; then
		generalUsage
		verifyErrorFlag "generalUsage(): Check 2nd parameter"
	fi
	if [ $LOCAL_TOTAL_PARAMETERS -ne 2 ]; then
		generalUsage
		verifyErrorFlag "generalUsage(): verify number of parameters (2)"
	fi
	;;
*)
	generalUsage
	verifyErrorFlag "generalUsage(): Check LOCAL_OPERATION parameter"
	;;
esac


# Start Script functionalities

case $LOCAL_OPERATION in
"suspend")
	# clear dmesg buffer on suspend
	dmesg -c > /dev/null
	wakeupTimer $LOCAL_WAKEUP_TIMER
	verifyErrorFlag "wakeupTimer(): timer value $LOCAL_WAKEUP_TIMER"
	case $LOCAL_ENVIRONMENT_TYPE in
	"android")
		if [ $LOCAL_SUSPEND_PATH = "force" ]; then
			showInfo "ANDROID: suspending system through suspend path" \
				 "\tstarting Android early suspend"
			releaseWakelocks $LOCAL_SYSTEM_WAKELOCKS
			verifyErrorFlag "releaseWakelocks(): $LOCAL_SYSTEM_WAKELOCKS"
			androidPowerKey
		elif [ $LOCAL_SUSPEND_PATH = "idle" ]; then
			showInfo "ANDROID: Suspending system through idle path" \
				 "\tstarting Android early suspend"
			releaseWakelocks $LOCAL_SYSTEM_WAKELOCKS
			verifyErrorFlag "releaseWakelocks(): $LOCAL_SYSTEM_WAKELOCKS"
		fi
		;;
	"kernel")
		if [ $LOCAL_SUSPEND_PATH = "force" ]; then
			releaseWakelocks $LOCAL_SYSTEM_WAKELOCKS
			verifyErrorFlag "releaseWakelocks(): $LOCAL_SYSTEM_WAKELOCKS"
			showInfo "KERNEL: suspending the system through suspend path"
			echo -n mem > $SYSFS_POWER_STATE

		elif [ $LOCAL_SUSPEND_PATH = "idle" ]; then
			showInfo "KERNEL: suspending the system through idle path"
			releaseWakelocks $LOCAL_SYSTEM_WAKELOCKS
			verifyErrorFlag "releaseWakelocks(): $LOCAL_SYSTEM_WAKELOCKS"
		fi
		;;
	*)
		generalUsage
		verifyErrorFlag	"generalUsage(): testing [android|kernel] for suspend"
	esac
	;;
"resume")
	while [ 1 ]; do
		if [ `dmesg | grep -wc "$HSR_SUSPEND_RESUME_MESSAGE_SUCCESS"` -gt 0 ]; then
			break
		fi
	done
	case $LOCAL_ENVIRONMENT_TYPE in
	"android")
		sleep 2
		showInfo "\tANDROID: resuming the system" \
			 "\tstarting Android late resume"
		androidPowerKey
		sleep 2
		androidMenukey
		holdWakelock $LOCAL_WAKELOCK
		verifyErrorFlag "holdWakelock(): name $LOCAL_WAKELOCK"
		;;
	"kernel")
		showInfo "\tKERNEL: resuming the system"
		holdWakelock $LOCAL_WAKELOCK
		verifyErrorFlag "holdWakelock(): name $LOCAL_WAKELOCK"
		;;
	*)
		generalUsage
		verifyErrorFlag "generalUsage(): testing [android|kernel] for resume"
	esac
	;;
esac

handlerDebugFileSystem.sh "umount"
exit $LOCAL_ERROR

# End of file

