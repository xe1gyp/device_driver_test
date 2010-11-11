#!/bin/bash

#
# Android Monkey handler
# @author: Leed Aguilar
#
# For more information about Android Monkey, visit:
# http://developer.android.com/guide/developing/tools/monkey.html
#

# =============================================================================
# Variables
# =============================================================================

LOCAL_INPUT_DEVICE=$1
LOCAL_REPEAT=$2
LOCAL_CMD_DELAY=$3
LOCAL_KEY_EVENT=$4
LOCAL_X_COORD=$4
LOCAL_Y_COORD=$5
LOCAL_ERROR=0

# =============================================================================
# Functions
# =============================================================================

# Available Monkey Functions:

# A) DispatchPointer(long downTime,  long eventTime, int action,
#  		  float x, float y, float pressure, float size, int metaState,
#		  float xPrecision, float yPrecision, int device, int edgeFlags)
#
# B) DispatchTrackball same as DispatchPointer
#
# C) DispatchKey(long downTime, long eventTime, int action, int code,
#	      int repeat, int metaState, int device, int scancode)
#
# D) DispatchFlip(boolean keyboardOpen)
#
# E) DispatchPress(int keyCode)
#
# F) LaunchActivity(String pkg_name, String cl_name)
#
# G) UserWait(long sleeptime)
#
# H) LongPress()

# Creates a keypad script for Android Monkey
# @ Function  : createPressKeyScript
# @ parameters: {androidKeyCode} {delay}
# @ Return    : None
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
# @ Function  : createTouchScreenScript
# @ parameters: {coordX} {coordY} {delay}
# @ Return    : None
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
# @ Function  : generalUsage
# @ parameters: None
# @ Return    : Error flag value
generalUsage() {
	cat <<-EOF >&1

	################################################################

	 Script Usage:

	 To excute a key event use the following:

	   $ handlerAndroidMonkey.sh keypad {repeat} {delay} {keyevent}

	 To excute a touchscreen event use the following:

	   $ handlerAndroidMonkey.sh touchscreen {repeat} {delay} {x coord} {y coord}

	   Where:
	   @ keyevent = Android UI KeyEvent**
	   @ x coord  = X coordinate value in the display
	   @ y coord  = Y coordinate value in the display
	   @ repeat   = Repeat the event N times
	   @ delay    = Android User delay for any operation

	   ** Check Android UI Key Codes at:
	   frameworks/base/include/ui/KeycodeLabels.h

	################################################################
	EOF
	LOCAL_ERROR=1
}

# Prints a message with a specific format
# @ Function  : showInfo
# @ Parameters: <message to display>
# @ Return    : None
showInfo() {
	echo -e "\n\n-------------------- handlerAndroidMonkey --------------------\n"
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
		handlerError.sh "log" "1" "halt" "handlerAndroidMonkey.sh"
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

# Check Parameters and scritp usage

# Check 1st parameter and total number of parameters
if [[ "$LOCAL_INPUT_DEVICE" = "keypad" && $# -ne 4 ]]; then
	generalUsage
	verifyErrorFlag
fi

# Check 1st parameter and total number of parameters
if [[ "$LOCAL_INPUT_DEVICE" = "touchscreen" && $# -ne 5 ]]; then
	generalUsage
	verifyErrorFlag "generalUsage(): Checking 1st parameter"
fi

# Check 2nd parameter
isPositiveInteger $LOCAL_REPEAT
verifyErrorFlag "generalUsage(): Checking 2nd parameter"

# Check 3rd  parameter
isPositiveInteger $LOCAL_CMD_DELAY
verifyErrorFlag "generalUsage(): Checking 3rd parameter"

# Check 4th and 5th parameter for keypad and touchscreen cases
# toucscreen = LOCAL_X_COORD
# keypad     = LOCAL_KEY_EVENT
case $LOCAL_INPUT_DEVICE in
"keypad")
	echo `expr match $LOCAL_KEY_EVENT 'KeyCode'` > match_value
	if [ `cat match_value` = "7" ]; then
		eval "eval 'echo "\$$LOCAL_KEY_EVENT"'" > tmp_val
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
	isPositiveInteger $LOCAL_Y_COORD
	verifyErrorFlag "generalUsage(): Checking 4th parameter for touchscreen case"
	isPositiveInteger $LOCAL_Y_COORD
	verifyErrorFlag "generalUsage(): Checking 5th parameter for touchscreen case"
	;;
esac

# Run Android Monkey commands

case $LOCAL_INPUT_DEVICE in

"keypad")
	createPressKeyScript $LOCAL_KEY_EVENT $LOCAL_CMD_DELAY
	# execute monkey command
	monkey -f keypadScript $LOCAL_REPEAT 2> err
	if [ $? -gt 0 ]; then
		showInfo "ERROR: Monkey command failed for the following reason:\n" "`cat err`"
		LOCAL_ERROR=1
		verifyErrorFlag "Android monkey execution for keypad"
	fi
	;;
"touchscreen")
        createPressKeyScript $LOCAL_X_COORD $LOCAL_Y_COORD $LOCAL_CMD_DELAY
        # execute monkey command
        monkey -f touchscreenScript $LOCAL_REPEAT 2> err
	if [ $? -gt 0 ]; then
		showInfo "ERROR: Monkey command failed for the following reason:\n" "`cat err`"
		LOCAL_ERROR=1
		verifyErrorFlag "Android monkey execution for touchscreen"
	fi
	;;
*)
	generalUsage
	verifyErrorFlag "generalUsage(): verifying LOCAL_INPUT_DEVICE parameter"
	;;
esac

exit $LOCAL_ERROR

# End of file
