#!/bin/sh

# Testsuite variables
export POSTFIX=`date "+%Y%m%d-%H%M%S"`
export TESTROOT=${PWD}
export TESTBIN=${PWD}/../bin
export MODDIR=${PWD}/../mods
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
export PATH="${TESTROOT}:${TESTBIN}:${TESTSCRIPT}:${PATH}"
export TC_SCENARIO="${TESTROOT}/scenarios"
export SCENARIO_NAMES=""
export STRESS=""

# Utilities
export UTILBIN=${TESTROOT}/../../utils/bin
export WAITKEY=$UTILBIN/akey

# General variables
export DMESG_FILE=/var/log/dmesg
export CHIP_NAME=twl

# Keypad devfs node
TEMP_EVENT=`ls /dev/input/ | grep event`
set $TEMP_EVENT

for i in $TEMP_EVENT
do
	${TESTBIN}/dev_name /dev/input/$i | grep -i "twl4030" | grep -i "keypad"
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

# End of file
