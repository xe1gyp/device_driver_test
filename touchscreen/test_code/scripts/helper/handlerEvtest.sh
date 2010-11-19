#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_OPERATION=$1
LOCAL_DEVFS_NODE=$2
LOCAL_NUMBER_ITERATIONS=$3

LOCAL_TEMP_FILE=$TOUCHSCREEN_DIR_TMP/touchscreen.temp.file

# =============================================================================
# Functions
# =============================================================================

# None

# =============================================================================
# Main
# =============================================================================

handlerError.sh "test"
if [ $? -eq 1 ]; then
	exit 1
fi

if [ "$LOCAL_DEVFS_NODE" = "$TOUCHSCREEN_DEVFS_PRIMARY" ]; then
	LOCAL_CONTROLLER="Primary"
elif [ "$LOCAL_DEVFS_NODE" = "$TOUCHSCREEN_DEVFS_SECONDARY" ]; then
	LOCAL_CONTROLLER="Secondary"
fi

if [ "$LOCAL_OPERATION" = "run" ]; then

	echo -e "\n\n\nINFO: Please interact with $LOCAL_CONTROLLER Touchscreen in 5 seconds"
	echo -e "INFO: Working with $LOCAL_DEVFS_NODE\n\n\n"

	sleep 5

	$TOUCHSCREEN_DIR_BINARIES/evtest $LOCAL_DEVFS_NODE $LOCAL_NUMBER_ITERATIONS

elif [ "$LOCAL_OPERATION" = "test" ]; then

	LOCAL_MODE=$4

	if [ "$LOCAL_MODE" = "noevents" ]; then

		echo -e "\n\n\nINFO: Please DO NOT INTERACT with $LOCAL_CONTROLLER Touchscreen, wait for process to finish"
		echo -e "INFO: Working with $LOCAL_DEVFS_NODE\n\n\n"
		sleep 5
		eval $TOUCHSCREEN_DIR_BINARIES/evtest $LOCAL_DEVFS_NODE $LOCAL_NUMBER_ITERATIONS | grep Event | grep time | grep type | grep code > $LOCAL_TEMP_FILE &
		sleep 10
		test -s $LOCAL_TEMP_FILE
		if [ $? -eq 0 ]; then
			echo "FATAL: Events were present under $LOCAL_DEVFS_NODE, please check!"
			exit 1
		fi

	else

		echo -e "\n\n\nINFO: Please interact with $LOCAL_CONTROLLER Touchscreen in 5 seconds"
		echo -e "INFO: Working with $LOCAL_DEVFS_NODE, no events will be displayed"
		sleep 5
		echo -e "INFO: Now!\n\n\n"
		eval $TOUCHSCREEN_DIR_BINARIES/evtest $LOCAL_DEVFS_NODE $LOCAL_NUMBER_ITERATIONS | grep Event | grep time | grep type | grep code > $LOCAL_TEMP_FILE &
		sleep 10
		test -s $LOCAL_TEMP_FILE
		if [ $? -eq 1 ]; then
			echo "FATAL: Events were not present under $LOCAL_DEVFS_NODE, please check!"
			exit 1
		fi
	fi

else
	echo "Unsupported Operation"
	exit 1
fi

# End of file
