#!/bin/sh

# TestSuite General Variables
export TOUCHSCREEN_POSTFIX=`date "+%Y%m%d-%H%M%S"`
export TOUCHSCREEN_ROOT=`pwd`

export TOUCHSCREEN_DIR_BINARIES=${TOUCHSCREEN_ROOT}/../binaries
export TOUCHSCREEN_DIR_HELPER=${TOUCHSCREEN_ROOT}/helper
export TOUCHSCREEN_DIR_TMP=${TOUCHSCREEN_ROOT}/tmp
export TOUCHSCREEN_DIR_TEST=${TOUCHSCREEN_ROOT}/test
export TOUCHSCREEN_DIR_SCENARIOS="${TOUCHSCREEN_ROOT}/scenarios"

export TOUCHSCREEN_FILE_OUTPUT=${TOUCHSCREEN_ROOT}/output.$TOUCHSCREEN_POSTFIX
export TOUCHSCREEN_FILE_LOG=${TOUCHSCREEN_ROOT}/log.$TOUCHSCREEN_POSTFIX
export TOUCHSCREEN_FILE_TMP=${TOUCHSCREEN_DIR_TMP}/tmp.$TOUCHSCREEN_POSTFIX
export TOUCHSCREEN_FILE_CMD=cmd.$TOUCHSCREEN_POSTFIX

export TOUCHSCREEN_DURATION=""
export TOUCHSCREEN_PRETTY_PRT=""
export TOUCHSCREEN_VERBOSE=""
export TOUCHSCREEN_SCENARIO_NAMES=""
export TOUCHSCREEN_STRESS=""

export PATH="${TOUCHSCREEN_ROOT}:${TOUCHSCREEN_DIR_HELPER}:${PATH}"

# Utils General Variables
. ${TOUCHSCREEN_ROOT}/../../utils/configuration/general.configuration
export UTILS_DIR_BIN=${TOUCHSCREEN_ROOT}/../../utils/bin
export UTILS_DIR_HANDLERS=${TOUCHSCREEN_ROOT}/../../utils/handlers

# General variables
export DMESG_FILE=/var/log/dmesg

# Keypad devfs node
TEMP_EVENT=`ls /sys/class/input/ | grep event`
set $TEMP_EVENT

for i in $TEMP_EVENT
do
	cat /sys/class/input/$i/device/modalias | grep "tm12xx"
	IS_THIS_OUR_DRIVER=`echo $?`
	if [ "$IS_THIS_OUR_DRIVER" -eq "0" ]
	then
		export DEVFS_TOUCHSCREEN=/dev/input/$i
		echo "Keypad node is " $DEVFS_TOUCHSCREEN
	fi
done

# Hardcode touchscreen primary controller for now
export DEVFS_TOUCHSCREEN=/dev/input/event1

if [ ! -e "$DEVFS_TOUCHSCREEN" ]
then
	echo "FATAL: Touchscreen node cannot be found -> $DEVFS_TOUCHSCREEN"
	exit 1
fi

# End of file
