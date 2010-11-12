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
LOCAL_OPERAND1=$2
LOCAL_OPERAND2=$3
LOCAL_OPERAND3=$4
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
	showInfo "\t      PRESSING POWER KEY"
	# Simulate press END_CALL key
	# OMAP4/3 Scancode for END_CALL key is 107
	sendevent /dev/input/event0 1  107 0
	sendevent /dev/input/event0 1  107 1
	sendevent /dev/input/event0 1  107 0
}

# Simulate the Menu key (screen unlock) press action
# @ Function  : androidMenuKey
# @ Parameters: None
# @ Return    : None
androidMenukey() {
	showInfo "\t      PRESSING MENU KEY" \
		 "\tunlocking Android User Interface"
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
	echo -e "\n--------------------- ANDROID WAKELOCKS ---------------------\n"
	for index in ${!wakelock_list[@]}; do
		echo ${wakelock_list[$index]} > $SYSTEM_WAKE_UNLOCK
		if [ `cat $SYSTEM_WAKE_LOCK | grep -wc ${wakelock_list[$index]}` -gt 0 ]; then
			echo -e "\tFAIL: <${wakelock_list[$wakelock]}> wakelock was NOT released\n"
		else
			echo -e "\tSUCCESS: <${wakelock_list[$wakelock]}> wakelock  was released\n"
		fi
	done
	echo -e "\n-------------------------------------------------------------\n"
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
		showInfo "SUCESS: Wakelock <$wakelock_name> was registered"
	else
		showInfo "ERROR: Wakelock <$eakelock_name> was NOT registered"
		LOCAL_ERROR=1
	fi
}

# Set the System Wakeup timer
# @ Function  : setWakeUpTimer
# @ Parameters: <timer value in seconds>
# @ Return    : Error flag value
setWakeUpTimer() {
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

	    handlerAndroidPM.sh [OPERATION] [OPTIONS

	    Where [OPERATION] can be:
	    A) suspend
	    B) resume
	    C) wakelock

	    A) To perform a suspend task do the following:

	       Try - handlerAndroidPM.sh suspend {android|kernel}
	                                         {force|idle}
	                                         {timer}
	       Where:
	       @ android = perform Android early suspend
	       @ kernel  = execute kernel global suspend
	       @ force   = perform a suspend through suspend path
	       @ idle    = perform a suspend through idle path
	       @ timer   = how long you want to keep the system is
	                    suspend state

	    B) To perform a resume task do the following:

	       Try - handlerAndroidPM.sh resume {android|kernel}
	       Where:
	       @ android = perform Android late resume
	       @ kernel  = wakeup the system till kernel prompt

	    C) To perform a wakelock operations do the following:

	       Try - handlerAndroidPM.sh wakelock {hold|release} {name}
	       Where:
	       @ hold    = to hold a wakelock
	       @ release = to release a wakelock
	       @ name    = name of the wakelock


	###############################################################"

	EOF
	LOCAL_ERROR=1
}

# Prints a message with a specific format
# @ Function  : showInfo
# @ Parameters: <message to display>
# @ Return    : None
showInfo() {
	echo -e "\n\n---------------------- handlerAndroidPM ----------------------\n"
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

# Check parameters and scritp usage

case $LOCAL_OPERATION in
"suspend")
	# Check 2nd parameter
	if [ `echo "android kernel" | grep -wc "$LOCAL_OPERAND1"` -ne 1 ]; then
		generalUsage
		verifyErrorFlag "generalUsage(): 2nd parameter is incorrect"
	fi
	# Check 3rd parameter
	if [ `echo "force idle" | grep -wc "$LOCAL_OPERAND2"` -ne 1 ]; then
		generalUsage
		verifyErrorFlag "generalUsage(): 3rd parameter is incorrect"
	fi
	# Check 4th parameter
	isPositiveInteger $LOCAL_OPERAND3
	verifyErrorFlag "generalUsage(): 4th parameter is incorrect"
	# Verify number of parameters
	if [ $LOCAL_TOTAL_PARAMETERS -ne 4 ]; then
		generalUsage
		verifyErrorFlag "generalUsage(): verify number of parameters (4)"
	fi
	;;
"resume")
	# Check 2nd parameter
	if [ `echo "android kernel" | grep -wc "$LOCAL_OPERAND1"` -ne 1 ]; then
		generalUsage
		verifyErrorFlag "generalUsage(): 2nd parameter is incorrect"
	fi
	# Verify number of parameters
	if [ $LOCAL_TOTAL_PARAMETERS -ne 2 ]; then
		generalUsage
		verifyErrorFlag "generalUsage(): verify number of parameters (2)"
	fi
	;;
"wakelock")
	# Check 2nd parameter
	if [ `echo "hold release" | grep -wc "$LOCAL_OPERAND1"` -ne 1 ]; then
		generalUsage
		verifyErrorFlag "generalUsage(): 2nd parameter is incorrect"
	fi
	# Verify number of parameters
	if [ $LOCAL_TOTAL_PARAMETERS -ne 3 ]; then
		generalUsage
		verifyErrorFlag "generalUsage(): verify number of parameters (3)"
	fi
	;;
*)
	generalUsage
	verifyErrorFlag "generalUsage(): main operation is not valid"
	;;
esac


# Start Script functionalities

case $LOCAL_OPERATION in
"suspend")
	# Set correspond variables
	LOCAL_ENVIRONMENT_TYPE=$LOCAL_OPERAND1
	LOCAL_SUSPEND_PATH=$LOCAL_OPERAND2
	LOCAL_WAKEUP_TIMER=$LOCAL_OPERAND3
	# Clear dmesg buffer on suspend
	dmesg -c > /dev/null
	setWakeUpTimer $LOCAL_WAKEUP_TIMER
	verifyErrorFlag "setWakeUpTimer(): timer value $LOCAL_WAKEUP_TIMER"
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
	esac
	;;
"resume")
	# Set correspond variables
	LOCAL_ENVIRONMENT_TYPE=$LOCAL_OPERAND1
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
	esac
	;;
"wakelock")
	# Set correspond variables
	LOCAL_WAKELOCK_STATUS=$LOCAL_OPERAND1
	LOCAL_WAKELOCK_NAME=$LOCAL_OPERAND2
	if [ $LOCAL_WAKELOCK_STATUS = "hold" ]; then
		holdWakelock $LOCAL_WAKELOCK_NAME
		verifyErrorFlag "holdWakelock(): Not able to set wakelock"
	elif [ $LOCAL_WAKELOCK_STATUS = "release" ]; then
		releaseWakelocks $LOCAL_WAKELOCK_NAME
		verifyErrorFlag "releaseWakelocks(): Not able to relese wakelock"
	fi
	;;
esac

handlerDebugFileSystem.sh "umount"
exit $LOCAL_ERROR

# End of file

