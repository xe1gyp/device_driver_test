#!/bin/bash

#
#  Android Activity Manager handler
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

# Android Activity Manager handler: This handler is able to open
# any Android app by registering the Intent of the process using
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

# TODO List
# 1. Include more Android processes as needed
# 2. Add more Android Errors and Warnings cases

# =============================================================================
# Local Variables
# =============================================================================

android_usecase=$1
execution_status=$2
multimedia_file=$3
error_val=0
need_scard=0
need_mm_file=0
sd_mount_point="/mnt/sdcard/"

# Android Errors and Warnings
am_warning="Warning: Activity not started"
am_error_class="Error: Activity class"
am_error_name="Error: Bad component name"
am_error_file="Failed to open file"
am_error_prepare="prepare failed"
am_error_param="unsupported parameter"
am_error_timeout="Launch timeout has expired"
am_error_crash="Activity destroy timeout for HistoryRecord"
am_error_open_file="Failed to open file for playback"

# Android Shell
am_shell="/system/bin/sh"
am_process="/system/bin/am"
am_process_execute="$am_shell $am_process  start -n "


# Android Processes
android_media_process="com.cooliris.media"
android_music_process="com.android.music"
android_omap4_cam_process="com.ti.omap4.android.camera"
android_cam_process="com.android.camera"
android_sound_recorder_process="com.android.soundrecorder"
android_gears_process="luabear.gears4android"
android_browser_process="com.android.browser"
android_setings_process="com.android.settings"
android_launcher_process="com.android.launcher"
android_wallpaper_picker="com.android.wallpaper.livepicker"

# Android APPs
app_gallery="$android_media_process/.Gallery"
app_movie_view="$android_media_process/.MovieView"
app_omap4_camera="$android_omap4_cam_process/.Camera"
app_video_camera="$android_omap4_cam_process/.VideoCamera"
app_camera="$android_cam_process/.Camera"
app_sound_recorder="$android_sound_recorder_process/.SoundRecorder"
app_media_playback="$android_music_process/.MediaPlaybackActivity"
app_gears4android="$android_gears_process/.Gears4Android"
app_browser="$android_browser_process/.BrowserActivity"
app_setting="$android_setings_process/.Settings"
app_wallpaper_chooser="$android_launcher_process/com.android.launcher2.WallpaperChooser"
app_live_wallpaper_picker="$android_wallpaper_picker/.LiveWallpaperListActivity"

# Execute APP commands
run_media_playback="$am_process_execute $app_media_playback"
run_sound_recorder="$am_process_execute $app_sound_recorder"
run_movie_view="$am_process_execute $app_movie_view"
run_video_camera="$am_process_execute $app_video_camera"
run_omap4_camera="$am_process_execute $app_omap4_camera"
run_camera="$am_process_execute $app_camera"
run_gallery="$am_process_execute $app_gallery"
run_gears4android="$am_process_execute $app_gears4android"
run_browser="$am_process_execute $app_browser"
run_settings="$am_process_execute $app_setting"
run_wallpaper_chooser="$am_process_execute $app_wallpaper_chooser"
run_live_wallpaper_picker="$am_process_execute $app_live_wallpaper_picker"

usecase_list=( "audio_playback" "audio_record" "av_playback" \
			"av_record" "image_capture" "image_capture_omap4" \
			"image_decode" "gears4android" "browser" "settings" \
			"wallpaper2D" "wallpaper3D" )

# =============================================================================
# Functions
# =============================================================================

# Display the script usage
# @ Function: usage
# @ parameters: None
# @ Return: Error flag value
usage() {
	echo -e "\n################# handlerActivityManager #################\n"
	echo -e "  Usage: Try one of the folllowing options:\n"
	echo -e "    - handlerActivityManager.sh <usecase> [start] <file>\n"
	echo -e "    - handlerActivityManager.sh <usecase> [stop]\n"
	echo -e "\n  Where <usecase> can be one of the following:\n"
	for index in ${!usecase_list[*]}; do
		echo -d "\t** ${usecase_list[$index]}"
	done
	echo -e "\n  Third parameter <file> is optional, if it is given\n" \
		" it can be used by some of the Android apps\n"
	echo -e "\n################# handlerActivityManager #################\n"
	error_val=1
}

# Detect if sdcard is mounted
# Android sdcard mount point: /mnt/sdcard
# Android creates a symlink: sdcard -> /mnt/sdcard
# @ Function: detectSdCard
# @ Parameters: None
# @ Return: Error flag value
detectSdCard() {
	if [ `df | grep -ic sdcard` -gt 0 ]; then
		showInfo "DEBUG: SDCARD is mounted"
	else
		showInfo "ERROR: SDCARD is not mounted" 1>&2
		error_val=1
	fi
}

# Find a file in an specific location
# @ Function: findFile
# @ Parameters: <file name> <location>
# @ Return: Error flag value
findFile() {
	file=$1
	location=$2
	# Verify that "location" exist
	if [ ! -d $location ]; then
		showInfo "ERROR: $location path does not exist" 1>&2
		error_val=1
		return
	fi
	# Search file in the path specified
	if [ `ls $location | grep -wc $file` -gt 0 ]; then
		showInfo "DEBUG: $file file found in $location"
	else
		showInfo "ERROR: $file file is not found, we can not proceed" 1>&2
		error_val=1
	fi
}

# Execute an Android process and search for errors
# and warnings when the process is being registered
# @ Function: executeAndroidProcess
# @ Parameters: <Android process>
# @ Return: Error flag value
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
	if [ `cat app_logcat | grep -rc "$am_error_crash"` -gt 0 ]; then
		showInfo "ERROR: The application $android_intent" \
			 "has stopped unexpectedly" 1>&2
		error_val=1
		return
	elif [ `cat app_err | grep -rc "$am_warning"` -gt 0  ]; then
		showInfo "WARNING: app is already running" 1>&2
		# Keeping this warning as inonfensive by now
		error_val=0
		return
	elif [ `cat app_err  | grep -rc "$am_error_class"` -gt 0 ]; then
		showInfo "ERROR: Activity class does not exist" 1>&2
		error_val=1
		return
	elif [ `cat app_err | grep -rc "$am_error_name"` -gt 0 ]; then
		showInfo "ERROR: bad component name" 1>&2
		error_val=1
		return
	elif [ `cat app_err  | grep -rc "$am_error_file"` -gt 0 ]; then
		showInfo "ERROR: Failed to open file" 1>&2
		error_val=1
		return
	elif [ `cat app_err  | grep -rc "$am_error_prepare"` -gt 0 ]; then
		showInfo "ERROR: prepare failed: -17" 1>&2
		error_val=1
		return
	elif [ `cat app_err  | grep -rc "$am_error_param"` -gt 0 ]; then
		showInfo "ERROR: unsupported parameter" 1>&2
		error_val=1
		return
	elif [ `cat app_err  | grep -rc "$am_error_timeout"` -gt 0 ]; then
		showInfo "ERROR: unsupported parameter" 1>&2
		error_val=1
		return
	elif [ `cat app_err  | grep -rc "$am_error_open_file"` -gt 0 ]; then
		showInfo "ERROR: Failed to open file for playback" 1>&2
		error_val=1
		return
	else
		showInfo "DEBUG: Testing $android_usecase"
	fi
}

# Prints a message with a specific format
# @ Function: showInfo
# @ Parameters: <message to display>
# @ Return: None
showInfo() {
	messages="$@"
	echo "[ handlerActivityManager ] $messages"
}

# Verify the value of the error flag
# if flag is set to '1' exit the script and register the failure
# The message parameter helps to debug the script
# @ Function: verifyErrorFlag
# @ Parameters: <debug message>
# @ Return: None
verifyErrorFlag() {
	debug_message=$1
	if [ $error_val -eq 1 ]; then
		handlerError.sh "log" "1" "halt" "handlerActivityManager.sh"
		showInfo "DEBUG: LOCAL ERROR DETECTED:" 1>&2
		showInfo "$debug_message"  1>&2
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

# Define Script Usage and validate all parameters
# Third parameter <file> is optional. It will be evaluated later

if [ ! `echo ${usecase_list[@]} | grep -wc $android_usecase` -eq 1 ]; then
	showInfo "USAGE: First parameter introduced is invalid" 1>&2
	usage
	verifyErrorFlag "usage(): evaluating 1st parameter"
elif [ ! `echo "start stop" | grep -wc $execution_status` -eq 1 ]; then
	showInfo "USAGE: Second parameter introduced is invalid" 1>&2
	usage
	verifyErrorFlag "usage(): evaluating 2nd parameter"
fi

# Run/Stop Android usecases

case $android_usecase in
"audio_playback")
	need_scard=1
	android_intent=$android_music_process
	execute=$run_media_playback
	;;
"audio_record")
	need_scard=1
	android_intent=$android_sound_recorder_process
	execute=$run_sound_recorder
	;;
"av_playback")
	need_scard=1
	android_intent=$android_media_process
	execute=$run_movie_view
	;;
"av_record")
	need_scard=1
	android_intent=$android_omap4_cam_process
	execute=$run_video_camera
	;;
"image_capture_omap4")
	need_scard=1
	android_intent=$android_omap4_cam_process
	execute=$run_omap4_camera
	;;
"image_capture")
	need_scard=1
	android_intent=$android_cam_process
	execute=$run_camera
	;;
"image_decode")
	need_scard=1
	android_intent=$android_media_process
	execute=$run_gallery
	;;
"gears4android")
	android_intent=$android_gears_process
	execute=$run_gears4android
	;;
"browser")
	android_intent=$android_browser_process
	execute=$run_browser
	;;
"settings")
	android_intent=$android_setings_process
	execute=$run_settings
	;;
"wallpaper2D")
	android_intent=$android_launcher_process
	execute=$run_wallpaper_chooser
	;;
"wallpaper3D")
	android_intent=$android_wallpaper_picker
	execute=$run_live_wallpaper_picker
	;;
*)
	usage
	verifyErrorFlag "usage(): Local process is not valid"
esac

case $execution_status in
"start")
	# Verify if the SDCARD is needed
	if [ $need_scard -eq 1 ]; then
		detectSdCard
		verifyErrorFlag "detectSdCard(): Verify if the SDCARD is needed"
	fi
	# If multimedia_file is set, verify it and use it
	if [[ ! -z $multimedia_file ]]; then
		findFile $multimedia_file $sd_mount_point
		verifyErrorFlag "Evaluating multimedia file parameter"
		# Let's use the file with the Android app
		new_exec_cmd="${execute} -d $sd_mount_point/$multimedia_file"
		execute=$new_exec_cmd
	fi
	executeAndroidProcess $execute
	verifyErrorFlag "executeAndroidProcess()"
	;;
"stop")
	killall $android_intent
	;;
*)
	usage
	verifyErrorFlag "usage(): expected [start|stop] action"
	;;
esac

exit $error_val

# End of file
