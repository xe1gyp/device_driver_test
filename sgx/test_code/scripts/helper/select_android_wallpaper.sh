#!/bin/bash

#
#  Select Android Wallpaper
#
#  Copyright (c) 2011 Texas Instruments
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

wallpaper_type=$1
select_wallpaper=$2
let wallpaper=$3-1
error_val=0

# Android error and warnings
export sgx_fail_to_load_wallpaper="Failure getting entry"

# =============================================================================
# Functions
# =============================================================================

# Prints a message with a specific format
# @ Function: showInfo
# @ Parameters: <message to display>
# @ Return: None
showInfo() {
	messages="$@"
	echo "[ SGX Wallpaper ] $messages"
}

# Display the script usage
# @ Function: usage
# @ parameters: None
# @ Return: Error flag value
usage() {
	#TODO: Implement script usage
	echo "USAGE"
	error_val=1
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
		handlerError.sh "log" "1" "halt" "select_android_wallpaper.sh"
		showInfo "DEBUG: LOCAL ERROR DETECTED:" "$debug_message"  1>&2
		exit $error_val
	fi
}

# =============================================================================
# Main
# =============================================================================

handlerError.sh "test"
if [ $? -eq 1 ]; then
	exit 1
fi

# TODO: implement script usage

case $wallpaper_type in
"2D")
	if [ "$select_wallpaper" = "multi" ]; then
		iterate=$wallpaper
		pos=0
	elif [ "$select_wallpaper" = "single" ]; then
		iterate=0
		pos=$wallpaper
	else
		usage
		verifyErrorFlag "Please verify script parameters"
	fi
	for i in $(eval echo "{0..$iterate}"); do
		let loc=$i+$pos
		# handlerAndroidMonkey.sh run 1 display.system.wallpaper
		handlerActivityManager.sh wallpaper2D start
		sleep 1
		handlerInputSubsystem.sh "keypad" "KeyCodeDpadRight"  1 $loc 0.5
		handlerInputSubsystem.sh "keypad" "KeyCodeDpadDown"   1 1 0.5
		handlerInputSubsystem.sh "keypad" "KeyCodeDpadCenter" 1 1 0.5

		# Verify that Resource Type process pass
		sleep 3
		# Save the android system buffer and then clear it
		logcat -d > ./wallpaper_android_log; logcat -c
		if [ `cat ./wallpaper_android_log | grep -rc \
				"$sgx_fail_to_load_wallpaper"` -gt 0 ]; then
			showInfo "ERROR: Android Wallpaper has failed"
			error_val=1
			verifyErrorFlag "Android Resource type has failed"
		fi
		rm wallpaper_android_log
		# Display Wallpaper for 5 seconds
		sleep 5
	done
	;;
"3D")
	if [ "$select_wallpaper" = "multi" ]; then
		iterate=$wallpaper
		pos=0
	elif [ "$select_wallpaper" = "single" ]; then
		iterate=0
		pos=$wallpaper
	else
		usage
		verifyErrorFlag "Please verify script parameters"
	fi
	for i in $(eval echo "{0..$iterate}"); do
		let loc=$i+$pos
		# handlerAndroidMonkey.sh run 1 display.system.wallpaper
		handlerActivityManager.sh "wallpaper3D" start
		sleep 1
		handlerInputSubsystem.sh "keypad" "KeyCodeDpadDown"  1 $loc 0.5
		handlerInputSubsystem.sh "keypad" "KeyCodeDpadCenter" 1 1 0.5
		# Verify that Resource Type process pass
		sleep 3
		# Save the android system buffer and then clear it
		logcat -d > ./wallpaper_android_log; logcat -c
		if [ `cat ./wallpaper_android_log | grep -rc \
				"$sgx_fail_to_load_wallpaper"` -gt 0 ]; then
			showInfo "ERROR: Android Wallpaper has failed"
			error_val=1
			verifyErrorFlag "Resource type has failed"
		fi
		handlerInputSubsystem.sh "keypad" "KeyCodeDpadDown"   1 1 0.5
		handlerInputSubsystem.sh "keypad" "KeyCodeDpadCenter" 1 1 0.5
		rm wallpaper_android_log
		# Display Wallpaper for 5 seconds
		sleep 5
	done
	;;
*)
	usage
	verifyErrorFlag "Please verify script parameters"
	;;
esac

exit $error_val

# End of file
