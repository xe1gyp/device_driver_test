#!/bin/bash

#
#  Android Monkey Tool handler
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

#
# For more information about Android Monkey, visit:
# http://developer.android.com/guide/developing/tools/monkey.html
#

# =============================================================================
# Local Variables
# =============================================================================

main_operation=$1
repeat_counter=$2
command_delay=$3
monkey_script=$3
key_event=$4
x_coord=$4
y_coord=$5
error_val=0

# =============================================================================
# Functions
# =============================================================================

# Creates a keypad script for Android Monkey
# @ Function: createPressKeyScript
# @ parameters: {androidKeyCode} {delay}
# @ Return: None
createPressKeyScript() {
	androidKeyCode=$1
	delay=$2
	keyCodeVal='eval "echo \$$androidKeyCode"'
	showInfo "MONKEY: keypad event: pressing $androidKeyCode = `eval $keyCodeVal`"
	# Creating Keypad script
	cat > keypadScript <<-EOF
	<pre>
	type= raw data
	count=10
	speed= 1
	start data >>
	captureDispatchKey(0,0,0,`eval $keyCodeVal`,0,0,0,0)
	captureDispatchKey(0,0,1,`eval $keyCodeVal`,0,0,0,0)
	UserWait($delay)
	EOF

}

# Creates a touchscreen script for Android Monkey
# @ Function: createTouchScreenScript
# @ parameters: {coordX} {coordY} {delay}
# @ Return: None
createTouchScreenScript() {
	coordX=$1
	coordY=$2
	delay=$3
	showInfo "MONKEY: touch event: pressing X=$coordX Y=$coordY"
	# Creating Touchscreen script
	cat > touchscreenScript <<-EOF
	<pre>
	type= raw data
	count=10
	speed= 1
	start data >>
	captureDispatchPointer(0,0,0,$coordX,$coordY,0,0,0,0,0,0,0)
	captureDispatchPointer(0,0,1,$coordX,$coordY,0,0,0,0,0,0,0)
	UserWait($delay)
	EOF
}

# Display the script usage
# @ Function: generalUsage
# @ parameters: None
# @ Return: Error flag value
generalUsage() {
	cat <<-EOF >&1

	################################################################

	 Script Usage:

	 To execute an Monkey script

	   $ handlerAndroidMonkey.sh run {repeat} <monkey script>

	 To excute a key event use the following:

	   $ handlerAndroidMonkey.sh keypad {repeat} {delay} {keyevent}

	 To excute a touchscreen event use the following:

	   $ handlerAndroidMonkey.sh touchscreen {repeat} {delay}
						 {x coord} {y coord}
	   Where:
	   @ keyevent = Android UI KeyEvent**
	   @ x coord  = X coordinate value in the display
	   @ y coord  = Y coordinate value in the display
	   @ repeat   = Repeat the event N times
	   @ delay    = Android User delay in any operation

	   ** Check Android UI Key Codes at:
	   frameworks/base/include/ui/KeycodeLabels.h

	################################################################
	EOF
	error_val=1
}

# Prints a message with a specific format
# @ Function: showInfo
# @ Parameters: <message to display>
# @ Return: None
showInfo() {
	messages="$@"
	echo -e "[ handlerAndroidMonkey ] $messages"
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
		handlerError.sh "log" "1" "halt" "handlerAndroidMonkey.sh"
		showInfo "DEBUG: LOCAL ERROR DETECTED:" "$debug_message"  1>&2
		exit $error_val
	fi
}

# Verify is the parameter is a valid number (integer)
# @ Function: isPositiveInteger
# @ Parameters: <number>
# @ Return: Error flag value
isPositiveInteger() {
	num=$1
	if ! [[ $num =~ ^[0-9]+$ ]]; then
		showInfo "ERROR: $num is not a number" 1>&2
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

# Check Parameters and scritp usage

case $main_operation in
"keypad")
	if [ $# -ne 4 ]; then
		generalUsage
		verifyErrorFlag "Number of parameters for 'keypad' operation is incorrect"
	fi
	# Check 2nd parameter
	isPositiveInteger $repeat_counter
	verifyErrorFlag "generalUsage(): Checking 2nd parameter"
	# Check 3rd  parameter
	isPositiveInteger $command_delay
	verifyErrorFlag "generalUsage(): Checking 3rd parameter"
	echo `expr match $key_event 'KeyCode'` > match_value
	if [ `cat match_value` = "7" ]; then
		eval "eval 'echo "\$$key_event"'" > tmp_val
		key_value=`cat tmp_val`
		if [ -z $key_value ]; then
			showInfo "ERROR: Introduce a valid KeyEvent"
			generalUsage
			verifyErrorFlag "generalUsage(): Checking 4th parameter for keypad case"
		fi
	else
		showInfo "ERROR: Introduce a valid KeyEvent"
		generalUsage
		verifyErrorFlag "generalUsage(): Checking 4th parameter for keypad case"
	fi
	;;
"touchscreen")
	if [ $# -ne 5 ]; then
		generalUsage
		verifyErrorFlag "Number of parameters for 'touchscreen' operation is incorrect"
	fi
	# Check 2nd parameter
	isPositiveInteger $repeat_counter
	verifyErrorFlag "generalUsage(): Checking 2nd parameter"
	# Check 3rd  parameter
	isPositiveInteger $command_delay
	verifyErrorFlag "generalUsage(): Checking 3rd parameter"
	# Check 4th parameter
	isPositiveInteger $x_coord
	verifyErrorFlag "generalUsage(): Checking 4th parameter for touchscreen case"
	# Check 5th parameter
	isPositiveInteger $y_coord
	verifyErrorFlag "generalUsage(): Checking 5th parameter for touchscreen case"
	;;
"run")
	if [ $# -ne 3 ]; then
		generalUsage
		verifyErrorFlag "Number of parameters for 'run' operation is incorrect"
	fi
	;;
*)
	generalUsage
	verifyErrorFlag "generalUsage(): Checking 1st parameter"
	;;
esac

# Run Android Monkey commands

case $main_operation in

"keypad")
	createPressKeyScript $key_event $command_delay
	# execute monkey command
	monkey -f keypadScript $repeat_counter 2> err
	if [ $? -gt 0 ]; then
		showInfo "ERROR: Monkey command failed for the following reason:\n" "`cat err`"
		error_val=1
		verifyErrorFlag "Android monkey execution for keypad"
	fi
	;;
"touchscreen")
        createPressKeyScript $x_coord $y_coord $command_delay
        # execute monkey command
        monkey -f touchscreenScript $repeat_counter 2> err
	if [ $? -gt 0 ]; then
		showInfo "ERROR: Monkey command failed for the following reason:\n" "`cat err`"
		error_val=1
		verifyErrorFlag "Android monkey execution for touchscreen"
	fi
	;;
"run")
	monkey -f $monkey_script $repeat_counter 2> err
	if [ $? -gt 0 ]; then
		showInfo "ERROR: Monkey command failed for the following reason:\n" "`cat err`"
		error_val=1
		verifyErrorFlag "Android monkey execution failed"
	fi
	;;
*)
	generalUsage
	verifyErrorFlag "generalUsage(): verifying main_operation parameter"
	;;
esac

exit $error_val

# End of file
