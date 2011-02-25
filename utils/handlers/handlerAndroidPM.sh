#!/bin/bash

#
#  Android Suspend and Resume Handler
#
#  Copyright (c) 2010 Texas Instruments
#
#  Author: Leed Aguilar <leed.aguilar@ti.com>
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License as
#  published by the Free Software Foundation; either version 2 of the
#  License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
#  USA
#

# =============================================================================
# Local Variables
# =============================================================================

operation=$1
operand1=$2
operand2=$3
operand3=$4
error_val=0
total_parameters=$#
wakelock='android_test_wakelock'
system_wakelocks=`cat $SYSTEM_WAKE_LOCK`

# =============================================================================
# Functions
# =============================================================================

# This function release all the wakelocks
# given in the wakelock list
# @ Function: releaseWakelocks
# @ Parameters: <wakelock list>
# @ Return: None
releaseWakelocks() {
	wakelocks=( "$@" )
	# Release all the wakelocks under /sys/power/wake_lock
	echo -e "\n################### Wakelock Manager ###################\n"
	for index in ${!wakelocks[@]}; do
		# Do not release PowerManagerService wakelock as it is a
		# system wakelock and it will expire automatically
		if [ ! ${wakelocks[$index]} = "PowerManagerService" ]; then
			echo ${wakelocks[$index]} > $SYSTEM_WAKE_UNLOCK
			if [ `cat $SYSTEM_WAKE_LOCK | \
				grep -wc ${wakelocks[$index]}` -gt 0 ]; then
				echo -e " Fail: <${wakelocks[$index]}> " \
						"wakelock was not released\n"
				error_val=1
			else
				echo -e " Success: <${wakelocks[$index]}> " \
						"wakelock was released\n"
			fi
		fi
	done
	echo -e "################### Wakelock Manager ###################\n"
}

# Hold a wakelock to keep the system awake
# @ Function: holdWakelock
# @ Parameters: <wakelock name>
# @ Return: Error flag value
holdWakelock() {
	wakelock_to_hold=$1
	echo $wakelock_to_hold > $SYSTEM_WAKE_LOCK
	# Verify that the wakelock is registered
	if [ `cat $SYSTEM_WAKE_LOCK | grep -wc $wakelock_to_hold` -gt 0 ]; then
		showInfo "SUCCESS: Wakelock <$wakelock_to_hold> was registered"
	else
		showInfo "ERROR: Wakelock <$wakelock_to_hold> wasn't registered"
		error_val=1
	fi
}

# Set the System Wakeup timer
# @ Function: setWakeUpTimer
# @ Parameters: <timer value in seconds>
# @ Return: Error flag value
setWakeUpTimer() {
	timer_value=$1
	handlerSysFs.sh set $PM_WAKEUP_TIMER_SECONDS "$timer_value"
	handlerSysFs.sh compare $PM_WAKEUP_TIMER_SECONDS "$timer_value"
	if [ $? = 1 ]; then
		showInfo "ERROR: wakeup timer was not registered"
		error_val=1
	else
		showInfo "SUCCESS: wakeup timer of $timer_value" \
					 "seconds was registered"
	fi
}

# Display the script usage
# @ Function: generalUsage
# @ parameters: None
# @ Return: Error flag value
usage() {
	cat <<-EOF >&1

	####################### handlerAndroidPM.sh #######################"

	SCRIPT USAGE:

	    handlerAndroidPM.sh [OPERATION] {OPTIONS}

	    Where [OPERATION] can be:
	    A) suspend
	    B) resume
	    C) wakelock

	    A) To perform a suspend task do the following:

	       Try - handlerAndroidPM.sh suspend {android|kernel}
	                                         {force|timeout}
	                                         {timer}
	       Where:
	       @ android = perform Android early suspend
	       @ kernel  = execute kernel global suspend
	       @ force   = perform a suspend through suspend path
	       @ timeout = perform a suspend through timeout path
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


	####################### handlerAndroidPM.sh #######################"

	EOF
	error_val=1
}

# Prints a message with a specific format
# @ Function: showInfo
# @ Parameters: <message to display>
# @ Return: None
showInfo() {
	messages=( "$@" )
	for index in ${!messages[@]}; do
		echo -e "[ handlerAndroidPM ] ${messages[$index]}"
	done
}

# Verify error_val flag
# if flag is set to '1' exit the script and register the failure
# The message parameter helps to debug the script
# @ Function: verifyErrorFlag
# @ Parameters: <debug message>
# @ Return: None
verifyErrorFlag() {
	debug_message=$1
	if [ $error_val -eq 1 ]; then
		handlerError.sh "log" "1" "halt" "handlerAndroidPM.sh"
		handlerDebugFileSystem.sh "umount"
		showInfo "Debug: local error detected:" "$debug_message"  1>&2
		exit $error_val
	fi
}

# Verify is the parameter is a valid number (integer)
# @ Function: isPositiveInteger
# @ Parameters: <number>
# @ Return: Error flag value
isPositiveInteger() {
	val=$1
	if ! [[ $val =~ ^[0-9]+$ ]]; then
		showInfo "ERROR: $val is not a number" 1>&2
		error_val=1
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

case $operation in
"suspend")
	# Verify number of parameters
	if [ $total_parameters -ne 4 ]; then
		generalUsage
		verifyErrorFlag "Verify number of parameters (4)"
	fi
	# Check 2nd parameter
	if [ `echo "android kernel" | grep -wc "$operand1"` -ne 1 ]; then
		generalUsage
		verifyErrorFlag "2nd parameter is incorrect"
	fi
	# Check 3rd parameter
	if [ `echo "force timeout" | grep -wc "$operand2"` -ne 1 ]; then
		generalUsage
		verifyErrorFlag  "3rd parameter is incorrect"
	fi
	# Check 4th parameter
	isPositiveInteger $operand3
	verifyErrorFlag "4th parameter is incorrect"

	;;
"resume")
	# Verify number of parameters
	if [ $total_parameters -ne 2 ]; then
		generalUsage
		verifyErrorFlag "Verify number of parameters (2)"
	fi
	# Check 2nd parameter
	if [ `echo "android kernel" | grep -wc "$operand1"` -ne 1 ]; then
		generalUsage
		verifyErrorFlag "usage(): 2nd parameter is incorrect"
	fi
	;;
"wakelock")
	# Verify number of parameters
	if [ $total_parameters -ne 3 ]; then
		generalUsage
		verifyErrorFlag "usage(): verify number of parameters (3)"
	fi
	# Check 2nd parameter
	if [ `echo "hold release" | grep -wc "$operand1"` -ne 1 ]; then
		generalUsage
		verifyErrorFlag "usage(): 2nd parameter is incorrect"
	fi
	;;
*)
	generalUsage
	verifyErrorFlag "usage(): parameters introduced are not valid"
	;;
esac


# Start Script functionalities

case $operation in
"suspend")
	# Set correspond variables
	environment_type=$operand1
	suspend_method=$operand2
	wakeup_timer=$operand3

	# Clear dmesg buffer on suspend
	dmesg -c > /dev/null
	setWakeUpTimer $wakeup_timer
	verifyErrorFlag "setWakeUpTimer(): timer value $wakeup_timer"

	case $environment_type in
	"android")
		if [ $suspend_method = "force" ]; then
			showInfo "Suspending system through suspend path" \
				 "Starting Android early suspend"
			releaseWakelocks $system_wakelocks
			verifyErrorFlag "Fail to release wakelock list"
			handlerInputSubsystem.sh "keypad" "KeyCodeEndCall" 1 1 1
		elif [ $suspend_method = "timeout" ]; then
			showInfo "Suspending system via timeout" \
				 "Starting Android early suspend"
			releaseWakelocks $system_wakelocks
			verifyErrorFlag "Fail to release wakelock list"
		fi
		;;
	"kernel")
		if [ $suspend_method = "force" ]; then
			releaseWakelocks $system_wakelocks
			verifyErrorFlag "Fail to release wakelock list"
			showInfo "Suspending the system through suspend path"
			echo -n mem > $SYSFS_POWER_STATE
		elif [ $suspend_method = "timeout" ]; then
			showInfo "Suspending the system via timeout"
			releaseWakelocks $system_wakelocks
			verifyErrorFlag "Fail to release wakelock list"
		fi
		;;
	esac
	;;
"resume")
	# Set correspond variables
	environment_type=$operand1
	while [ 1 ]; do
		if [ `dmesg -c | grep -wc \
			"$HSR_SUSPEND_RESUME_MESSAGE_SUCCESS"` -gt 0 ]; then
			break
		fi
		sleep 1
	done
	case $environment_type in
	"android")
		holdWakelock $wakelock
		verifyErrorFlag "Not able to set wakelock"
		showInfo "Android: resuming the system" \
			 "starting Android late resume"
		handlerInputSubsystem.sh "keypad" "KeyCodeEndCall" 1 1 1
		sleep 1
		handlerInputSubsystem.sh "keypad" "KeyCodeF1" 1 1 1
		;;
	"kernel")
		showInfo "Kernel: resuming the system"
		holdWakelock $wakelock
		verifyErrorFlag "Not able to set wakelock"
		;;
	esac
	;;
"wakelock")
	# Set correspond variables
	wakelock_status=$operand1
	wakelock_name=$operand2
	if [ $wakelock_status = "hold" ]; then
		holdWakelock $wakelock_name
		verifyErrorFlag "Not able to set wakelock"
	elif [ $wakelock_status = "release" ]; then
		releaseWakelocks $wakelock_name
		verifyErrorFlag "Not able to release wakelock"
	fi
	;;
esac

handlerDebugFileSystem.sh "umount"
exit $error_val

# End of file

