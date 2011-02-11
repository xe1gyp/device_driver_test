#!/bin/bash

#
#  Android Input Subsystem Handler
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

device=$1
param2=$2
param3=$3
param4=$4
param5=$5
param6=$6
error_val=0
total_parameters=$#

# =============================================================================
# Functions
# =============================================================================

# Generate key press events
# @ Function: generateKeyPress
# @ Parameters: <keyCode | press type | repeat | delay >
# @ Return: None
generateKeyPress() {
	keyCode=$1
	pressType=$2
	repeat=$3
	delay=$4
	counter=0
	keyCodeVal='eval "echo \$$keyCode"'
	showInfo "keypad event: pressing $keyCode = `eval $keyCodeVal`"
	showInfo "generating $repeat key events every $delay seconds"
	while [ $counter -lt $repeat ]; do
		sendevent $omapKeypadEvent 1 `eval $keyCodeVal` $pressType
		sendevent $omapKeypadEvent 0 0 0
		sendevent $omapKeypadEvent 1 `eval $keyCodeVal` 0
		sendevent $omapKeypadEvent 0 0 0
		sleep $delay
		counter=`expr $counter + 1`
	done
}

# Generate touch events in Synaptic TouchPoint 1
# @ Function: generateScreenTouch1
# @ Parameters: < x & y coord | press type | repeat | delay>
# @ Return: None
generateScreenTouch1() {
	xcoord=$1
	ycoord=$2
	pressType=$3
	repeat=$4
	delay=$5
	counter=0
	showInfo "touch event: pressing $xcoord & $ycoord coordinates"
	showInfo "generating $repeat touch events every $delay seconds"
	while [ $counter -lt $repeat ]; do
		sendevent $touchPoint1Event 1 330 $pressType
		sendevent $touchPoint1Event 3 0 $xcoord
		sendevent $touchPoint1Event 3 1 $ycoord
		sendevent $touchPoint1Event 0 0 0
		sendevent $touchPoint1Event 1 330 0
		sendevent $touchPoint1Event 0 0 0
		sleep $delay
		counter=`expr $counter + 1`
	done
}

# Generate touch events in Synaptic TouchPoint 2
# @ Function: generateScreenTouch2
# @ Parameters: < x & y coord | press type | repeat | delay>
# @ Return: None
generateScreenTouch2() {
	xcoord=$1
	ycoord=$2
	pressType=$3
	repeat=$4
	delay=$5
	counter=0
	showInfo "touch event: pressing $xcoord & $ycoord coordinates"
	showInfo "generating $repeat touch events every $delay seconds"
	while [ $counter -lt $repeat ]; do
		sendevent $touchPoint2Event 1 330 $pressType
		sendevent $touchPoint2Event 3 0 $xcoord
		sendevent $touchPoint2Event 3 1 $ycoord
		sendevent $touchPoint2Event 0 0 0
		sendevent $touchPoint2Event 1 330 0
		sendevent $touchPoint2Event 0 0 0
		sleep $delay
		counter=`expr $counter + 1`
	done
}

# Display the script usage
# @ Function: generalUsage
# @ parameters: None
# @ Return: Error flag value
usage() {
	cat <<-EOF >&1

	################### handlerInputSubsystem.sh ###################"

	Script Usage:

	A) handlerInputSubsystem.sh "keypad" [keyCode] <press type>
				     <repeat> <delay>

	B) handlerInputSubsystem.sh "TouchPoint1" <X coord> <Y coord>
				     <press type> <repeat> <delay>

	C) handlerInputSubsystem.sh "TouchPoint2" <X coord> <Y coord>
				     <press type> <repeat> <delay>

	To be implemented:

	D) handlerInputSubsystem.sh "pressure"
	E) handlerInputSubsystem.sh "sensors"
	F) handlerInputSubsystem.sh "ambientLight"
	G) handlerInputSubsystem.sh "accelerometer"
	H) handlerInputSubsystem.sh "headsetJack"
	I) handlerInputSubsystem.sh "proximity"

	Where parameters are:

	@ keyCode: key code decimal value
	@ X|Y coordinates values on the screen
	@ press type: 1-short press, 2-long press
	@ repeat: how many repetitions?
	@ delay: time between repetitions

	################### handlerInputSubsystem.sh ###################"

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
		echo -e "[ handlerInputSubsystem ] ${messages[$index]}"
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
		handlerError.sh "log" "1" "halt" "handlerInputSubsystem.sh"
		handlerDebugFileSystem.sh "umount"
		showInfo "ERROR: $debug_message"  1>&2
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

# TODO: Check parameters and scritp usage

# Start Script functionalities

case $device in
"keypad")
	showInfo "Device: OMAP 4430 Keypad"
	generateKeyPress $param2 $param3 $param4 $param5
	;;
"TouchPoint1")
	showInfo "Synaptics Touchscreen 1"
	generateScreenTouch1 $param2 $param3 $param4 $param5 $param6
	;;
"TouchPoint2")
	showInfo "Synaptics Touchscreen 2"
	generateScreenTouch2 $param2 $param3 $param4 $param5 $param6
	;;
"sensors")
	showInfo "Sensors: to be implemented"
	;;
"pressure")
	showInfo "Pressure Sensor: to be implemented"
	;;
"ambientLight")
	showInfo "Ambient Light Sensor: to be implemented"
	;;
"accelerometer")
	showInfo "Accelerometer Sensor: to be implemented"
	;;
"headsetJack")
	showInfo "headset Jack: to be implemented"
	;;
"proximity")
	showInfo "Proximity Sensor: to be implemented"
	;;
*)
	usage
	verifyErrorFlag
esac

handlerDebugFileSystem.sh "umount"
exit $error_val

# End of file
