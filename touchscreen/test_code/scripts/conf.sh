#!/bin/sh

# TestSuite General Variables
export TOUCHSCREEN_POSTFIX=`date "+%Y%m%d-%H%M%S"`
export TOUCHSCREEN_ROOT=`pwd`

export TOUCHSCREEN_DIR_BINARIES=${TOUCHSCREEN_ROOT}/../bin
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

export PATH="${PATH}:${TOUCHSCREEN_ROOT}:${TOUCHSCREEN_DIR_BINARIES}:${TOUCHSCREEN_DIR_HELPER}"

# Utils General Variables
export UTILS_DIR=${TOUCHSCREEN_ROOT}/../../utils/
export UTILS_DIR_BIN=${TOUCHSCREEN_ROOT}/../../utils/bin
export UTILS_DIR_HANDLERS=${TOUCHSCREEN_ROOT}/../../utils/handlers
export UTILS_DIR_SCRIPTS=${TOUCHSCREEN_ROOT}/../../utils/scripts

. ${TOUCHSCREEN_ROOT}/../../utils/configuration/general.configuration

export PATH="$PATH:$UTILS_DIR_BIN:$UTILS_DIR_HANDLERS:$UTILS_DIR_SCRIPTS"

# General variables
# None

if [ `cat /proc/cpuinfo| grep -ic "OMAP4"` -eq 1 ]; then
	export TOUCHSCREEN_SYSFS_NAME="Synaptic TM12XX"
	export TOUCHSCREEN_IRQ_NAME="tm12xx_ts"
fi

# Touchscreen devfs node
TEMP_EVENT=`ls /sys/class/input/ | grep event`
set $TEMP_EVENT

LOCAL_COUNTER=0
for i in $TEMP_EVENT
do
	cat /sys/class/input/$i/device/name | grep -i "$TOUCHSCREEN_SYSFS_NAME"
	if [ $? -eq 0 ]
	then
		echo "Touchscreen node is /dev/input/$i"
		LOCAL_COUNTER=`expr $LOCAL_COUNTER + 1`

		if [ $LOCAL_COUNTER -eq 1 ]; then
			export TOUCHSCREEN_DEVFS_PRIMARY=/dev/input/$i
		elif  [ $LOCAL_COUNTER -eq 2 ]; then
			export TOUCHSCREEN_DEVFS_SECONDARY=/dev/input/$i
		fi
	fi
done

echo "INFO: Total number of touchscreen controllers found: $LOCAL_NUMBER_OF_INTERFACE"

if [ ! -e "$TOUCHSCREEN_DEVFS_PRIMARY" ]
then
	echo "FATAL: Touchscreen node cannot be found -> $TOUCHSCREEN_DEVFS_PRIMARY"
	exit 1
fi

# Touchscreen irq number
export TOUCHSCREEN_IRQ_NUMBER=$TOUCHSCREEN_DIR_TMP/touchscreen.irq.number
export TOUCHSCREEN_IRQ_INITIAL=$TOUCHSCREEN_DIR_TMP/touchscreen.irq.initial
export TOUCHSCREEN_IRQ_FINAL=$TOUCHSCREEN_DIR_TMP/touchscreen.irq.final

TOUCHSCREEN_IRQ_TEMP=`handlerIrq.sh "get" "irq" "$TOUCHSCREEN_IRQ_NAME" "$TOUCHSCREEN_IRQ_NUMBER"`

set $TOUCHSCREEN_IRQ_TEMP
LOCAL_COUNTER=0
for i in $TOUCHSCREEN_IRQ_TEMP
do
	LOCAL_COUNTER=`expr $LOCAL_COUNTER + 1`
  if [ $LOCAL_COUNTER -eq 1 ]; then
		export TOUCHSCREEN_IRQ_PRIMARY=$i
	elif [ $LOCAL_COUNTER -eq 2 ]; then
		export TOUCHSCREEN_IRQ_SECONDARY=$i
	fi
done

# End of file
