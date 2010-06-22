#!/bin/sh

# Testsuites variables
export POSTFIX=`date "+%Y%m%d-%H%M%S"`
export TESTROOT=${PWD}
export TESTBIN=${PWD}/../bin
export UTILBIN=${PWD}/../../utils/bin
export UTILSCRIPTS=${PWD}/../../utils/scripts
export TESTMODS=${PWD}/../modules
export TESTSCRIPT=${PWD}/helper
export TMPBASE=${TESTROOT}/tmp
export TMPFILE=${TMPBASE}/tmp.$POSTFIX
export CMDFILE=cmd.$POSTFIX
export TESTDIR=${TESTROOT}/test
export PRETTY_PRT=""
export VERBOSE=""
export OUTPUTFILE=${TESTROOT}/output.$POSTFIX
export LOGFILE=${TESTROOT}/log.$POSTFIX
export DURATION=""
export PATH="${PATH}:${TESTROOT}:${TESTBIN}:${TESTSCRIPT}"
export TC_SCENARIO="${TESTROOT}/scenarios"
export SCENARIO_NAMES=""

# General variables
export CHIP_NAME=twl
export PROCFS_RTC=/proc/driver/rtc
export UTILS_DIR_HANDLERS=${TESTROOT}/../../utils/handlers

. ${TESTROOT}/../../utils/configuration/general.configuration

export PATH="$PATH:$UTILS_DIR_HANDLERS:$UTILBIN"

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
$UTILS_DIR_HANDLERS/handlerIrq.sh "get" "rtc" "$IRQ_FILE"
export IRQ_VALUE=`cat $IRQ_FILE`

if [ "$IRQ_VALUE" == "" ]; then
	echo "Warning: Interrupt not found. Some test cases could be affected"
fi

if [ ! `echo 1+1 | bc` ]; then
	echo "FATAL: BC is unavailable, cannot continue"
	exit 1
fi

# End of file
