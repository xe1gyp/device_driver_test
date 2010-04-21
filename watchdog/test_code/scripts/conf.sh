#!/bin/sh

# Testsuite variables
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
export UTILS_DIR_HANDLERS=${TESTROOT}/../../utils/handlers

# Platform
export DMESG_FILE=/tmp/result.tmp
$TESTSCRIPT/fileSystemType.sh

# Driver specific
export WATCHDOG_DEVFS=/dev/watchdog
export WATCHDOG_MODALIAS=watchdog

# Test Applications
export WATCHDOG_KEEPALIVE=watchdog_keepalive
export WATCHDOG_STATUS=watchdog_status
export WATCHDOG_STATUS_BOOT=watchdog_status_boot
export WATCHDOG_SUPPORT=watchdog_support
export WATCHDOG_TIMEOUT_GET=watchdog_timeout_get
export WATCHDOG_TIMEOUT_SET=watchdog_timeout_set
export WATCHDOG_WRITE=watchdog_write
export WATCHDOG_KEEPALIVE_SMP=watchdog_keepalive_smp

#cat /sys/class/misc/watchdog/uevent
#MAJOR=10
#MINOR=130
#DEVNAME=watchdog
#PHYSDEVPATH=/devices/platform/omap_wdt
#PHYSDEVBUS=platform
#PHYSDEVDRIVER=omap_wdt

# End of file

