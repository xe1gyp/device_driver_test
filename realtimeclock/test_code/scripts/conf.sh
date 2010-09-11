#!/bin/sh

# TestSuite General Variables
export RTC_POSTFIX=`date "+%Y%m%d-%H%M%S"`
export RTC_ROOT=`pwd`

export RTC_DIR_BINARIES=${RTC_ROOT}/../bin
export RTC_DIR_HELPER=${RTC_ROOT}/helper
export RTC_DIR_TMP=${RTC_ROOT}/tmp
export RTC_DIR_TEST=${RTC_ROOT}/test
export RTC_DIR_SCENARIOS="${RTC_ROOT}/scenarios"

export RTC_FILE_OUTPUT=${RTC_ROOT}/output.$RTC_POSTFIX
export RTC_FILE_LOG=${RTC_ROOT}/log.$RTC_POSTFIX
export RTC_FILE_TMP=${RTC_DIR_TMP}/tmp.$RTC_POSTFIX
export RTC_FILE_CMD=cmd.$RTC_POSTFIX

export RTC_DURATION=""
export RTC_PRETTY_PRT=""
export RTC_VERBOSE=""
export RTC_SCENARIO_NAMES=""
export RTC_STRESS=""

export PATH="$PATH:$RTC_ROOT:$RTC_DIR_BINARIES:$RTC_DIR_HELPER"

# Utils General Variables
export UTILS_DIR=$RTC_ROOT/../../utils/
export UTILS_DIR_BIN=$UTILS_DIR/bin
export UTILS_DIR_HANDLERS=$UTILS_DIR/handlers
export UTILS_DIR_SCRIPTS=$UTILS_DIR/scripts

. $UTILS_DIR/configuration/general.configuration

export PATH="$PATH:$UTILS_DIR_BIN:$UTILS_DIR_HANDLERS:$UTILS_DIR_SCRIPTS"

# General variables
export CHIP_NAME=twl
export PROCFS_RTC=/proc/driver/rtc

# rtc devfs node autodetection
TEMP_EVENT=`ls /sys/class/rtc/ | grep rtc`
set $TEMP_EVENT

for i in $TEMP_EVENT
do
	cat /sys/class/rtc/$i/device/modalias | grep -i "rtc"
	IS_THIS_OUR_DRIVER=`echo $?`
	if [ "$IS_THIS_OUR_DRIVER" -eq "0" ]
	then
		export DEVFS_RTC=/dev/$i
		echo "Real Time Clock node is $DEVFS_RTC"
	fi
done

if [ ! -e "$DEVFS_RTC" ]
then
	echo "FATAL: Real Time Clock node cannot be found -> $DEVFS_RTC"
	exit 1
fi

# Application names
export APP_ALARM_GET=alarm_get
export APP_ALARM_GET_EVENT=alarm_get_event
export APP_ALARM_RESET=alarm_reset
export APP_ALARM_SET=alarm_set
export APP_DEVICE_OPEN_CLOSE=device_open_close
export APP_PER_INT=per_int
export APP_PER_INT_GET=per_int_get
export APP_PER_INT_GET1=per_int_get1
export APP_PER_INT_GET2=per_int_get2
export APP_PER_INT_OFF=per_int_off
export APP_PER_INT_ON=per_int_on
export APP_TIME_GET=time_get
export APP_TIME_SET=time_set
export APP_TIME_KEEP_READING=rtc_read
export APP_ALARM_KEEP_READING=alarm_read

# Return Messages
export MSG_OK_ALARM_RANG="Alarm Rang"
export MSG_OK_ALARM_RESET="Successful alarm set!"
export MSG_OK_ALARM_SET="Successful alarm reset!"
export MSG_DATE_TIME_SET_NEW="New RTC Date/Time:"
export MSG_DATE_TIME_USE_CURRENT="Current RTC Date/Time:"
export MSG_CANNOT_OPEN_DEVICE="Requested device cannot be opened!"

# Clock variables
export APP_VAL_INPUT_USER='0'
export APP_VAL_INPUT_DEFAULT='1'
export APP_VALID_TIME='121212'		# HHMMSS
export APP_VALID_DATE='112906'		# MMDDYY
export APP_INVALID_TIME1='257570'	# HHMMSS
export APP_INVALID_DATE1='133206'	# MMDDYY
export APP_INVALID_DATE2='022906'	# MMDDYY

export RTC_VALID_TIME='12:12:12'
export RTC_VALID_DATE='29-11-2006'
export DEF_RTC_VALID_TIME='23:59:45'
export DEF_RTC_VALID_TIME_PROCFS='23:59'
export DEF_RTC_VALID_DATE='31-12-2004'
export DEF_PROC_VALID_TIME='23:59:45'
export DEF_PROC_VALID_DATE='2004-12-31'

export APP_ALARM_REPEAT_VAL='100'

export PER_INT_SEC='1'
export ALARM_SEC='1'
export ALARM_VALUE=5

# IRQ value
IRQ_FILE=$TMPBASE/irq_temp
handlerIrq.sh "get" "irq" "rtc" "$IRQ_FILE"
export IRQ_VALUE=`cat $IRQ_FILE`

if [ "$IRQ_VALUE" = "" ]; then
	echo "Warning: Interrupt not found. Some test cases could be affected"
fi

if [ ! `echo 1+1 | bc` ]; then
	echo "FATAL: BC is unavailable, cannot continue"
	exit 1
fi

# End of file
