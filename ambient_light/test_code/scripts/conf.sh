#!/bin/sh -x

# TestSuite General Variables
export AMBIENT_LIGHT_POSTFIX=`date "+%Y%m%d-%H%M%S"`
export AMBIENT_LIGHT_ROOT=`pwd`

export AMBIENT_LIGHT_DIR_BINARIES=${AMBIENT_LIGHT_ROOT}/../bin
export AMBIENT_LIGHT_DIR_HELPER=${AMBIENT_LIGHT_ROOT}/helper
export AMBIENT_LIGHT_DIR_TMP=${AMBIENT_LIGHT_ROOT}/tmp
export AMBIENT_LIGHT_DIR_TEST=${AMBIENT_LIGHT_ROOT}/test
export AMBIENT_LIGHT_DIR_SCENARIOS="${AMBIENT_LIGHT_ROOT}/scenarios"

export AMBIENT_LIGHT_FILE_OUTPUT=${AMBIENT_LIGHT_ROOT}/output.$AMBIENT_LIGHT_POSTFIX
export AMBIENT_LIGHT_FILE_LOG=${AMBIENT_LIGHT_ROOT}/log.$AMBIENT_LIGHT_POSTFIX
export AMBIENT_LIGHT_FILE_TMP=${AMBIENT_LIGHT_DIR_TMP}/tmp.$AMBIENT_LIGHT_POSTFIX
export AMBIENT_LIGHT_FILE_CMD=cmd.$AMBIENT_LIGHT_POSTFIX

export AMBIENT_LIGHT_DURATION=""
export AMBIENT_LIGHT_PRETTY_PRT=""
export AMBIENT_LIGHT_VERBOSE=""
export AMBIENT_LIGHT_SCENARIO_NAMES=""
export AMBIENT_LIGHT_STRESS=""

export AMBIENT_LIGHT_MODE_MEAS400=2
export AMBIENT_LIGHT_MODE_MOTDET=4
export AMBIENT_LIGHT_RANGE_2G=2000
export AMBIENT_LIGHT_RANGE_8G=8000

export AMBIENT_LIGHT_SYSFS_PATH="/sys/bus/i2c/drivers/bh1780/3-0029"

export PATH="${PATH}:${AMBIENT_LIGHT_ROOT}:${AMBIENT_LIGHT_DIR_BINARIES}:${AMBIENT_LIGHT_DIR_HELPER}"

# Utils General Variables
export UTILS_DIR_BIN=${AMBIENT_LIGHT_ROOT}/../../utils/bin
export UTILS_DIR_HANDLERS=${AMBIENT_LIGHT_ROOT}/../../utils/handlers
export UTILS_DIR_SCRIPTS=${AMBIENT_LIGHT_ROOT}/../../utils/scripts

. ${AMBIENT_LIGHT_ROOT}/../../utils/configuration/general.configuration

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
