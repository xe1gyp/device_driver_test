#!/bin/sh

#
#  is_android_ready.sh
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

# This utility helps to know when the Android UI is available. It checks
# for the Android launcher process to be present and waits until the
# bootanimation binary is removed. Once these two conditions are met,
# the Android UI will be available.

# =============================================================================
# Local Variables
# =============================================================================

error_val=0
launcher_time=0
launcher_timeout=300
animation_time=0
animation_timeout=120
android_animation="/system/bin/bootanimation"
android_launcher="com.android.launcher"

# =============================================================================
# Functions
# =============================================================================

# Display the script usage
# @ Function: usage
# @ parameters: None
# @ Return: Error flag value
usage() {
	echo ""
	echo "---------------------------------------------"
	echo "usage: $0"
	echo "---------------------------------------------"
	echo ""
	error_val=1
}

# Prints a message with a specific format
# @ Function: showInfo
# @ Parameters: <message to display>
# @ Return: None
showInfo() {
	message=$1
	echo "[ Android Initialization ] $message"
}

# Verify error flag
# if flag is set to '1' exit the script and register the failure
# The message parameter helps to debug the script
# @ Function: verifyErrorFlag
# @ Parameters: <debug message>
# @ Return: None
verifyErrorFlag() {
	debug_message=$1
	if [ $error_val -eq 1 ]; then
		handlerError.sh "log" "1" "halt" "$0.sh"
		handlerDebugFileSystem.sh "umount"
		showInfo "Debug: Local error detected" 1>&2
		showInfo "$debug_message"  1>&2
		exit $error_val
	fi
}

# =============================================================================
# MAIN
# =============================================================================

# verify script usage
if [ $# -gt 1 ]; then
	usage 1>&2
	verifyErrorFlag "Script usage is incorrect"
fi

# Verify Androd UI is up and running

while [ `ps | grep -wc $android_launcher` -lt 2 ]; do
	if [ $launcher_time -gt $launcher_timeout ]; then
		# If Android Launcher is not loaded before the
		# timeout expired, exit and report failure
		showInfo "Error: $android_launcher was not loaded"
		error_val=1
		verifyErrorFlag "Error: Launcher timeout has expired"
	fi
	sleep 1
	let launcher_time=$launcher_time+1
done

showInfo "Android Launcher [$android_launcher] is running"

while [ `ps | grep -wc $android_animation` -gt 1 ]; do
	if [ $animation_time -gt $animation_timeout ]; then
		# If Android Animation process is not killed before
		# the timeout expired, exit and report failure
		showInfo "Error: $android_animation is still available"
		error_val=1
		verifyErrorFlag "Error: Animation timeout has expired"
	fi
	sleep 1
	let animation_time=$animation_time+1
done

showInfo "Android amimation process has been removed"
showInfo "Android UI is now available"

exit $error_val

# End of file
