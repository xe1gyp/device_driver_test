#!/bin/sh -x

# TestSuite General Variables
export LED_POSTFIX=`date "+%Y%m%d-%H%M%S"`
export LED_ROOT=`pwd`

export LED_DIR_BINARIES=${LED_ROOT}/../bin
export LED_DIR_HELPER=${LED_ROOT}/helper
export LED_DIR_TMP=${LED_ROOT}/tmp
export LED_DIR_TEST=${LED_ROOT}/test
export LED_DIR_SCENARIOS="${LED_ROOT}/scenarios"

export LED_FILE_OUTPUT=${LED_ROOT}/output.$LED_POSTFIX
export LED_FILE_LOG=${LED_ROOT}/log.$LED_POSTFIX
export LED_FILE_TMP=${LED_DIR_TMP}/tmp.$LED_POSTFIX
export LED_FILE_CMD=cmd.$LED_POSTFIX

export LED_DURATION=""
export LED_PRETTY_PRT=""
export LED_VERBOSE=""
export LED_SCENARIO_NAMES=""
export LED_STRESS=""

export LED_SYSFS_PATH="/sys/class/leds"

export PATH="${PATH}:${LED_ROOT}:${LED_DIR_BINARIES}:${LED_DIR_HELPER}"

# Utils General Variables
export UTILS_DIR=$LED_ROOT/../../utils/
export UTILS_DIR_BIN=$UTILS_DIR/bin
export UTILS_DIR_HANDLERS=$UTILS_DIR/handlers
export UTILS_DIR_SCRIPTS=$UTILS_DIR/scripts

. $UTILS_DIR/configuration/general.configuration

export PATH="$PATH:$UTILS_DIR_BIN:$UTILS_DIR_HANDLERS:$UTILS_DIR_SCRIPTS"

# General variables
export DMESG_FILE=/var/log/dmesg

# Keypad devfs node
TEMP_EVENT=`ls /sys/class/input/ | grep event`
set $TEMP_EVENT

for i in $TEMP_EVENT
do
	cat /sys/class/input/$i/device/name | grep "keypad"
	IS_THIS_OUR_DRIVER=`echo $?`
	if [ "$IS_THIS_OUR_DRIVER" -eq "0" ]
	then
		export DEVFS_KEYPAD=/dev/input/$i
		echo "Keypad node is " $DEVFS_KEYPAD
	fi
done

if [ ! -e "$DEVFS_KEYPAD" ]
then
	echo "Warning: Keypad node cannot be found -> $DEVFS_KEYPAD"
fi

# End of file
