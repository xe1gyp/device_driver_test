#!/bin/sh

# TestSuite General Variables
export WATCHDOG_POSTFIX=`date "+%Y%m%d-%H%M%S"`
export WATCHDOG_ROOT=`pwd`

export WATCHDOG_DIR_BINARIES=${WATCHDOG_ROOT}/../bin
export WATCHDOG_DIR_MODULES=${WATCHDOG_ROOT}/../modules
export WATCHDOG_DIR_HELPER=${WATCHDOG_ROOT}/helper
export WATCHDOG_DIR_TMP=${WATCHDOG_ROOT}/tmp
export WATCHDOG_DIR_TEST=${WATCHDOG_ROOT}/test
export WATCHDOG_DIR_SCENARIOS="${WATCHDOG_ROOT}/scenarios"

export WATCHDOG_FILE_OUTPUT=${WATCHDOG_ROOT}/output.$WATCHDOG_POSTFIX
export WATCHDOG_FILE_LOG=${WATCHDOG_ROOT}/log.$WATCHDOG_POSTFIX
export WATCHDOG_FILE_TMP=${WATCHDOG_DIR_TMP}/tmp.$WATCHDOG_POSTFIX
export WATCHDOG_FILE_CMD=cmd.$WATCHDOG_POSTFIX

export WATCHDOG_DURATION=""
export WATCHDOG_PRETTY_PRT=""
export WATCHDOG_VERBOSE=""
export WATCHDOG_SCENARIO_NAMES=""
export WATCHDOG_STRESS=""

export PATH="$PATH:$WATCHDOG_ROOT:$WATCHDOG_DIR_BINARIES:$WATCHDOG_DIR_HELPER"

# Utils General Variables
export UTILS_DIR=$WATCHDOG_ROOT/../../utils/
export UTILS_DIR_BIN=$UTILS_DIR/bin
export UTILS_DIR_HANDLERS=$UTILS_DIR/handlers
export UTILS_DIR_SCRIPTS=$UTILS_DIR/scripts

. $UTILS_DIR/configuration/general.configuration

export PATH="$PATH:$UTILS_DIR_BIN:$UTILS_DIR_HANDLERS:$UTILS_DIR_SCRIPTS"

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

