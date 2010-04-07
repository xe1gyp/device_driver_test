#!/bin/sh

# TestSuite General Variables
export KEYPAD_POSTFIX=`date "+%Y%m%d-%H%M%S"`
export KEYPAD_ROOT=`pwd`

export KEYPAD_DIR_BINARIES=${KEYPAD_ROOT}/../bin
export KEYPAD_DIR_HELPER=${KEYPAD_ROOT}/helper
export KEYPAD_DIR_TMP=${KEYPAD_ROOT}/tmp
export KEYPAD_DIR_TEST=${KEYPAD_ROOT}/test
export KEYPAD_DIR_SCENARIOS="${KEYPAD_ROOT}/scenarios"

export KEYPAD_FILE_OUTPUT=${KEYPAD_ROOT}/output.$KEYPAD_POSTFIX
export KEYPAD_FILE_LOG=${KEYPAD_ROOT}/log.$KEYPAD_POSTFIX
export KEYPAD_FILE_TMP=${KEYPAD_DIR_TMP}/tmp.$KEYPAD_POSTFIX
export KEYPAD_FILE_CMD=cmd.$KEYPAD_POSTFIX

export KEYPAD_DURATION=""
export KEYPAD_PRETTY_PRT=""
export KEYPAD_VERBOSE=""
export KEYPAD_SCENARIO_NAMES=""
export KEYPAD_STRESS=""

export PATH="${KEYPAD_ROOT}:${KEYPAD_DIR_BINARIES}:${KEYPAD_DIR_HELPER}:${PATH}"

# Utils General Variables
export UTILS_DIR_BIN=${KEYPAD_ROOT}/../../utils/bin
export UTILS_DIR_HANDLERS=${KEYPAD_ROOT}/../../utils/handlers
export UTILS_DIR_SCRIPTS=${KEYPAD_ROOT}/../../utils/scripts

. ${KEYPAD_ROOT}/../../utils/configuration/general.configuration

export PATH="$PATH:$UTILS_DIR_BIN:$UTILS_DIR_HANDLERS:$UTILS_DIR_SCRIPTS"

# General variables
export DMESG_FILE=/var/log/dmesg
export CHIP_NAME=twl

# Keypad devfs node
TEMP_EVENT=`ls /dev/input/ | grep event`
echo "Found 2 nodes available:"
echo  $TEMP_EVENT

set $TEMP_EVENT

for i in $TEMP_EVENT
do
	${KEYPAD_DIR_BINARIES}/dev_name /dev/input/$i | grep -i "keypad"
	IS_THIS_OUR_DRIVER=`echo $?`
	if [ "$IS_THIS_OUR_DRIVER" -eq "0" ]
	then
		export DEVFS_KEYPAD=/dev/input/$i
		echo "Keypad node is " $DEVFS_KEYPAD
		break
	fi
done

if [ ! -e "$DEVFS_KEYPAD" ]
then
	echo "FATAL: Keypad node cannot be found -> $DEVFS_KEYPAD"
	exit 1
fi

# Remove all existing references for handlerError.sh
handlerError.sh "clean"

# End of file

