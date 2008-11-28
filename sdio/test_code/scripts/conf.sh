#!/bin/sh
###################################################################################################################
# Configuration script
# Author  : Misael Lopez Cruz
# Date    : September 19, 2006
# Description: Configuration script with user specified value for some environment variables
###################################################################################################################


# Remove SDIO test module if it exists
lsmod | grep "sdio" && rmmod sdio.ko

# These variables dont' need to be modified
export POSTFIX=`date "+%Y%m%d-%H%M%S"`
export TESTROOT=${PWD}
export TESTBIN=${PWD}/../bin
export UTILBIN=${PWD}/../../utils/bin
export TESTMODS=${PWD}/../mods
export TESTSCRIPT=${PWD}/helper
export TMPBASE=${TESTROOT}/tmp
export TMPFILE=${TMPBASE}/tmp.$POSTFIX
export CMDFILE=cmd.$POSTFIX
export TESTDIR=${TESTROOT}/test
export PRETTY_PRT=""
export VERBOSE=""
export OUTPUTFILE=${TESTROOT}/output.$POSTFIX
export LOGFILE=${TESTROOT}/log.$POSTFIX
export DURATION="1h"
export PATH="${PATH}:${TESTROOT}:${TESTBIN}:${TESTSCRIPT}"
export TC_SCENARIO="${TESTROOT}/scenarios"
export SCENARIO_NAMES=""

# File system parameters
export FS_LOGFILE=/var/log/dmesg

# SDIO parameters
export SDIO_TYPE="SDIO WLAN card"
export SDIO_MANUF_FILE=$TESTSCRIPT/wlan.manuf
export SDIO_MOD=sdio.ko

# Kernel version
export KERNEL_RELEASE_LABEL_DIR=${TESTROOT}/../../utils/kernel_release_labels
export KERNEL_RELEASE_LABEL=`uname -r`
source $KERNEL_RELEASE_LABEL_DIR/$KERNEL_RELEASE_LABEL "sdio"
