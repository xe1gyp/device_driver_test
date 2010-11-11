#!/bin/bash
#
# Android Activity Manager handler: This handler is able to open
# any Android APP by registering the Intent of the process using
# the Activity Manager
#
# Processes supported:
# - com.ti.omap4.android.camera
# - com.android.camera
# - com.android.soundrecorder
# - com.android.music
# - com.cooliris.media
# - com.android.browser
# - luabear.gears4android
#
# For more information on the Android Activity Manager, visit:
# http://developer.android.com/reference/android/app/Activity.html
#
# @author: Leed Aguilar
#

# TODO List
# 1. Include more Android processes as needed
# 2. Add more Android Errors and Warnings cases

# =============================================================================
# Variables
# =============================================================================

LOCAL_PROCESS=$1
LOCAL_ACTION=$2
LOCAL_FILE=$3
LOCAL_ERROR=0
LOCAL_NEED_SDCARD=0
LOCAL_NEED_FILE=0
LOCAL_SDCARD_MOUNT_POINT="/mnt/sdcard/"

# Android Errors and Warnings
LOCAL_AM_WARNING="Warning: Activity not started"
LOCAL_AM_ERROR_CLASS="Error: Activity class"
LOCAL_AM_ERROR_NAME="Error: Bad component name"
LOCAL_AM_ERROR_FILE="Failed to open file"
LOCAL_AM_ERROR_PREPARE="prepare failed"
LOCAL_AM_ERROR_PARAM="unsupported parameter"
LOCAL_AM_ERROR_TIMEOUT="Launch timeout has expired"
LOCAL_AM_ERROR_CRASH="Activity destroy timeout for HistoryRecord"

# Android Shell
LOCAL_AM_SHELL="/system/bin/sh"
LOCAL_AM="/system/bin/am"
LOCAL_AM_EXECUTE="$LOCAL_AM_SHELL $LOCAL_AM  start -n "


# Android Processes
LOCAL_ANDROID_MEDIA="com.cooliris.media"
LOCAL_ANDROID_MUSIC="com.android.music"
LOCAL_ANDROID_OMAP4_CAMERA="com.ti.omap4.android.camera"
LOCAL_ANDROID_CAMERA="com.android.camera"
LOCAL_ANDROID_RECORDER="com.android.soundrecorder"
LOCAL_ANDROID_GEARS="luabear.gears4android"
LOCAL_ANDROID_BROWSER="com.android.browser"
LOCAL_ANDROID_SETTINGS="com.android.settings"

# Android APPs
LOCAL_APP_GALLERY="$LOCAL_ANDROID_MEDIA/.Gallery"
LOCAL_APP_AV_PLAYBACK="$LOCAL_ANDROID_MEDIA/.MovieView"
LOCAL_APP_OMAP4_CAMERA="$LOCAL_ANDROID_OMAP4_CAMERA/.Camera"
LOCAL_APP_VIDEO_CAMERA="$LOCAL_ANDROID_OMAP4_CAMERA/.VideoCamera"
LOCAL_APP_CAMERA="$LOCAL_ANDROID_CAMERA/.Camera"
LOCAL_APP_SOUND_RECORDED="$LOCAL_ANDROID_RECORDER/.SoundRecorder"
LOCAL_APP_AUDIO_PLAYBACK="$LOCAL_ANDROID_MUSIC/.MediaPlaybackActivity"
LOCAL_APP_GEARS4ANDROID="$LOCAL_ANDROID_GEARS/.Gears4Android"
LOCAL_APP_BROWSER="$LOCAL_ANDROID_BROWSER/.BrowserActivity"
LOCAL_APP_SETTINGS="$LOCAL_ANDROID_SETTINGS/.Settings"

# Execute APP commands
LOCAL_RUN_AUDIO_PLAYBACK="$LOCAL_AM_EXECUTE $LOCAL_APP_AUDIO_PLAYBACK"
LOCAL_RUN_AUDIO_RECORD="$LOCAL_AM_EXECUTE $LOCAL_APP_SOUND_RECORDED"
LOCAL_RUN_AV_PLAYBACK="$LOCAL_AM_EXECUTE $LOCAL_APP_AV_PLAYBACK"
LOCAL_RUN_AV_RECORD="$LOCAL_AM_EXECUTE $LOCAL_APP_VIDEO_CAMERA"
LOCAL_RUN_IMAGE_CAPTURE_OMAP4="$LOCAL_AM_EXECUTE $LOCAL_APP_OMAP4_CAMERA"
LOCAL_RUN_IMAGE_CAPTURE="$LOCAL_AM_EXECUTE $LOCAL_APP_CAMERA"
LOCAL_RUN_IMAGE_DECODE="$LOCAL_AM_EXECUTE $LOCAL_APP_GALLERY"
LOCAL_RUN_GEARS4ANDROID="$LOCAL_AM_EXECUTE $LOCAL_APP_GEARS4ANDROID"
LOCAL_RUN_BROWSER="$LOCAL_AM_EXECUTE $LOCAL_APP_BROWSER"
LOCAL_RUN_SETTINGS="$LOCAL_AM_EXECUTE $LOCAL_APP_SETTINGS"

LOCAL_ANDROID_USECASE=( "audio_playback" "audio_record" "av_playback" \
			"av_record" "image_capture" "image_capture_omap4" \
			"image_decode" "gears4android" "browser" "settings" )

# =============================================================================
# Functions
# =============================================================================

# Display the script usage
# @ Function  : usage
# @ parameters: None
# @ Return    : Error flag value
usage() {
	echo -e "\n-----------------------------------------------------\n"
	echo -e "  Usage: Try one of the folllowing options:\n"
	echo -e "    - handlerActivityManager.sh <process> [start] <file>\n"
	echo -e "    - handlerActivityManager.sh <process> [stop]\n"
	echo -e "\n  Where <process> can be one of the following:\n"
	for index in ${!LOCAL_MM_USECASE[*]}; do
		echo -d "\t** ${LOCAL_ANDROID_USECASE[$index]}"
	done
	echo -e "\n  Third parameter <file> is optional, if it is given\n" \
		" it can be used by some of the Android APPs\n"
	echo -e "\n-----------------------------------------------------\t"
	LOCAL_ERROR=1
}

# Detect if sdcard is mounted
# Android sdcard mount point: /mnt/sdcard
# Android creates a symlink: sdcard -> /mnt/sdcard
# @ Function  : detectSdCard
# @ Parameters: None
# @ Return    : Error flag value
detectSdCard() {
	if [ `df | grep -ic sdcard` -gt 0 ]; then
		showInfo "DEBUG: SDCARD is mounted"
	else
		showInfo "ERROR: SDCARD is not mounted" 1>&2
		LOCAL_ERROR=1
	fi
}

# Find a file in an specific location
# @ Function  : findFile
# @ Parameters: <file name> <location>
# @ Return    : Error flag value
findFile() {
	file=$1
	location=$2
	# Verify that "location" exist
	if [ ! -d $location ]; then
		showInfo "ERROR: $location path does not exist" 1>&2
		LOCAL_ERROR=1
		return
	fi
	# Search file in the path specified
	if [ `ls $location | grep -wc $file` -gt 0 ]; then
		showInfo "DEBUG: $file file found in $location"
	else
		showInfo "ERROR: $file file is not found, we can not proceed" 1>&2
		LOCAL_ERROR=1
	fi
}

# Execute an Android process and search for errors
# and warnings when the process is being registered
# @ Function  : executeAndroidProcess
# @ Parameters: <Android process>
# @ Return    : Error flag value
executeAndroidProcess() {
	android_process="$@"
	# Clear logcat buffer
	logcat -c
	# Execute process and save standard error
	sleep 4; $android_process 2> app_err
	# save logcat output of the execution process
	logcat -d > app_logcat
	# When an app crashes the error log is not sent to standard error
	# output, but it can be obtained from the logcat
	if [ `cat app_logcat | grep -rc "$LOCAL_AM_ERROR_CRASH"` -gt 0 ]; then
		showInfo "ERROR: The application $LOCAL_ANDROID_INTENT" \
			 "has stopped unexpectedly" 1>&2
		LOCAL_ERROR=1
		return
	elif [ `cat app_err | grep -rc "$LOCAL_AM_WARNING"` -gt 0  ]; then
		showInfo "WARNING: app is already running" 1>&2
		# Keeping this warning as inonfensive by now
		LOCAL_ERROR=0
		return
	elif [ `cat app_err  | grep -rc "$LOCAL_AM_ERROR_CLASS"` -gt 0 ]; then
		showInfo "ERROR: Activity class does not exist" 1>&2
		LOCAL_ERROR=1
		return
	elif [ `cat app_err | grep -rc "$LOCAL_AM_ERROR_NAME"` -gt 0 ]; then
		showInfo "ERROR: bad component name" 1>&2
		LOCAL_ERROR=1
		return
	elif [ `cat app_err  | grep -rc "$LOCAL_AM_ERROR_FILE"` -gt 0 ]; then
		showInfo "ERROR: Failed to open file" 1>&2
		LOCAL_ERROR=1
		return
	elif [ `cat app_err  | grep -rc "$LOCAL_AM_ERROR_PREPARE"` -gt 0 ]; then
		showInfo "ERROR: prepare failed: -17" 1>&2
		LOCAL_ERROR=1
		return
	elif [ `cat app_err  | grep -rc "$LOCAL_AM_ERROR_PARAM"` -gt 0 ]; then
		showInfo "ERROR: unsupported parameter" 1>&2
		LOCAL_ERROR=1
		return
	elif [ `cat app_err  | grep -rc "$LOCAL_AM_ERROR_TIMEOUT"` -gt 0 ]; then
		showInfo "ERROR: unsupported parameter" 1>&2
		LOCAL_ERROR=1
		return
	else
		showInfo "DEBUG: Testing $LOCAL_PROCESS"
	fi
}

# Prints a message with a specific format
# @ Function  : showInfo
# @ Parameters: <message to display>
# @ Return    : None
showInfo() {
	echo -e "\n\n------------------- handlerActivityManager -------------------\n"
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
		handlerError.sh "log" "1" "halt" "handlerActivityManager.sh"
		showInfo "DEBUG: LOCAL ERROR DETECTED:" "$debug_message"  1>&2
		exit $LOCAL_ERROR
	fi
}

# =============================================================================
# Main
# =============================================================================

handlerError.sh "test"
if [ $? -eq 1 ]; then
        exit 1
fi

# Define Script Usage and validate all parameters
# Third parameter <file> is optional. It will be evaluated later

if [ ! `echo ${LOCAL_ANDROID_USECASE[@]} | grep -wc $LOCAL_PROCESS` -eq 1 ]; then
	showInfo "USAGE: First parameter introduced is invalid" 1>&2
	usage
	verifyErrorFlag "usage(): evaluating 1st parameter"
elif [ ! `echo "start stop" | grep -wc $LOCAL_ACTION` -eq 1 ]; then
	showInfo "USAGE: Second parameter introduced is invalid" 1>&2
	usage
	verifyErrorFlag "usage(): evaluating 2nd parameter"
fi

# Run/Stop Android usecases

case $LOCAL_PROCESS in
"audio_playback")
	LOCAL_NEED_SDCARD=1
	LOCAL_ANDROID_INTENT=$LOCAL_ANDROID_MUSIC
	LOCAL_RUN=$LOCAL_RUN_AUDIO_PLAYBACK
	;;
"audio_record")
	LOCAL_NEED_SDCARD=1
	LOCAL_ANDROID_INTENT=$LOCAL_ANDROID_RECORDER
	LOCAL_RUN=$LOCAL_RUN_AUDIO_RECORD
	;;
"av_playback")
	LOCAL_NEED_SDCARD=1
	LOCAL_ANDROID_INTENT=$LOCAL_ANDROID_MEDIA
	LOCAL_RUN=$LOCAL_RUN_AV_PLAYBACK
	;;
"av_record")
	LOCAL_NEED_SDCARD=1
	LOCAL_ANDROID_INTENT=$LOCAL_ANDROID_OMAP4_CAMERA
	LOCAL_RUN=$LOCAL_RUN_AV_RECORD
	;;
"image_capture_omap4")
	LOCAL_NEED_SDCARD=1
	LOCAL_ANDROID_INTENT=$LOCAL_ANDROID_OMAP4_CAMERA
	LOCAL_RUN=$LOCAL_RUN_IMAGE_CAPTURE_OMAP4
	;;
"image_capture")
	LOCAL_NEED_SDCARD=1
	LOCAL_ANDROID_INTENT=$LOCAL_ANDROID_CAMERA
	LOCAL_RUN=$LOCAL_RUN_IMAGE_CAPTURE
	;;
"image_decode")
	LOCAL_NEED_SDCARD=1
	LOCAL_ANDROID_INTENT=$LOCAL_ANDROID_MEDIA
	LOCAL_RUN=$LOCAL_RUN_IMAGE_DECODE
	;;
"gears4android")
	LOCAL_ANDROID_INTENT=$LOCAL_ANDROID_GEARS
	LOCAL_RUN=$LOCAL_RUN_GEARS4ANDROID
	;;
"browser")
	LOCAL_ANDROID_INTENT=$LOCAL_ANDROID_BROWSER
	LOCAL_RUN=$LOCAL_RUN_BROWSER
	;;
"settings")
	LOCAL_ANDROID_INTENT=$LOCAL_ANDROID_SETTINGS
	LOCAL_RUN=$LOCAL_RUN_SETTINGS
	;;
*)
	usage
	verifyErrorFlag "usage(): Local process is not valid"
esac

case $LOCAL_ACTION in
"start")
	# Verify if the SDCARD is needed
	if [ $LOCAL_NEED_SDCARD -eq 1 ]; then
		detectSdCard
		verifyErrorFlag "detectSdCard(): Verify if the SDCARD is needed"
	fi
	# If LOCAL_FILE is set, verify it and use it
	if [[ ! -z $LOCAL_FILE ]]; then
		findFile $LOCAL_FILE $LOCAL_SDCARD_MOUNT_POINT
		verifyErrorFlag "findFile(): evaluating LOCAL_FILE parameter"
		# Let's use the file with the Android app
		new_execute_command="${LOCAL_RUN} -d $LOCAL_SDCARD_MOUNT_POINT/$LOCAL_FILE"
		LOCAL_RUN=$new_execute_command
	fi
	executeAndroidProcess $LOCAL_RUN
	verifyErrorFlag "executeAndroidProcess()"
	;;
"stop")
	killall $LOCAL_ANDROID_INTENT
	;;
*)
	usage
	verifyErrorFlag "usage(): expected [start|stop] action"
	;;
esac

exit $LOCAL_ERROR

# End of file
