#!/bin/sh

# Testsuite variables
export POSTFIX=`date "+%Y%m%d-%H%M%S"`
export TESTROOT=${PWD}
export TESTBIN=${PWD}/../bin
export TESTLIB=${PWD}/../lib
export TESTSCRIPT=${PWD}/helper
export MODDIR=${PWD}/../mod
export TMPBASE=${TESTROOT}/tmp
export TMPFILE=${TMPBASE}/tmp.$POSTFIX
export CMDFILE=cmd.$POSTFIX
export TESTDIR=${TESTROOT}/test
export PRETTY_PRT=""
export VERBOSE=""
export OUTPUTFILE=${TESTROOT}/output.$POSTFIX
export LOGFILE=${TESTROOT}/log.$POSTFIX
export DURATION=""
export PATH="${TESTROOT}:${TESTBIN}:${TESTSCRIPT}:${PATH}"
export TC_SCENARIO="${TESTROOT}/scenarios"
export SCENARIO_NAMES=""
export INTERACTIVE=""
export STRESS=""
export UTILBIN=${TESTROOT}/../../utils/bin

# Utilities
export WAIT_ANSWER=$UTILBIN/akey
export WAIT_KEY=$UTILBIN/waitkey

# General Variables
export DMESG_FILE=/var/log/dmesg
export DRIVER_NAME=ads7846

# Touchscreen devfs node
TEMP_EVENT=`ls /sys/class/input/ | grep event`
set $TEMP_EVENT

for i in $TEMP_EVENT
do
	TEMP_NAME=`cat /sys/class/input/$i/device/modalias`
	if [ $TEMP_NAME == $DRIVER_NAME ]
		then
			export DEVFS_TOUCHSCREEN=/dev/input/$i
			echo "Touchscreen node is " $DEVFS_TOUCHSCREEN
	fi
done

if [ ! -e "$DEVFS_TOUCHSCREEN" ]
	then
		echo "FATAL: Touchscreen node cannot be found -> $DEVFS_TOUCHSCREEN"
		exit 1
fi

# Specific touchscreen variables
export TSLIB_TSDEVICE=$DEVFS_TOUCHSCREEN
export TSLIB_CONFFILE=/etc/ts.conf
export TSLIB_PLUGINDIR=/lib/ts

# Tslib applications
export TS_PRINT_RAW=/bin/ts_print_raw
export TS_CALIBRATE=/bin/ts_calibrate
export TS_PRINT=/bin/ts_print
export TS_TEST=/bin/ts_test

if [ ! -f  $TS_CALIBRATE ]
then
	if [ ! -f $TS_PRINT_RAW ]
	then
		if [ ! -f $TS_PRINT ]
		then
			if [ ! -f $TS_TEST ]
			then
				echo "FATAL: tslib applications cannot be found"
				exit 1
			fi
		fi
	fi
fi

# Framebuffer specific
if [ -f /sys/power/fb_timeout_value ]
then
	export SYSFS_FB_TIMEOUT_VALUE=/sys/power/fb_timeout_value
else
	export SYSFS_FB_TIMEOUT_VALUE=
fi

# End of file
