#!/bin/sh

###############################################################################
# Configuration script
# Author  : Nishant Menon
# Date    : Tue Jun 13 12:23:42 CDT 2006
# Description: Configuration Script
###############################################################################

# These variables don't need to be modified
export POSTFIX=`date "+%Y%m%d-%H%M%S"`
export TESTROOT=${PWD}
export TESTBIN=${PWD}/../bin
export MODDIR=${PWD}/../modules
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

# Utilities
export UTILBIN=${TESTROOT}/../../utils/bin
export UTILSCRIPTS=${TESTROOT}/../../utils/scripts

# Adapters
export I2C_ADAPTERS="1 2 3"
export I2C_ADAPTER_1=1
export I2C_ADAPTER_2=2
export I2C_ADAPTER_3=3

# Operating mode
export STANDARD_MODE=100
export FAST_MODE=400
export HIGH_SPEED_MODE=2600

# Addresses
export I2C_ADDRESSES="0x48 0x49 0x4A 0x4B"

# Registers
export I2C_REG_1=0x49
export I2C_REG_1_VALUE_INITIAL=0x01
export I2C_REG_2=0x4A
export I2C_REG_2_VALUE_INITIAL=0xD4
export I2C_REG_INVALID=0x2D

# Irqs
export INT_24XX_I2C1_IRQ=56
export INT_24XX_I2C2_IRQ=57
export INT_34XX_I2C3_IRQ=61

# Test case IDs for runtestcase.sh script
export EXISTING_DEVICES=1
export MULTIPLE_DEVICES=2
export NON_EXISTING_DEVICES=3
export TRANSFER_CANCELLATION=4
export INTERRUPT_MODE=5

# General variables
export DMESG_PATH=/var/log/dmesg
export PROCFS_INTERRUPTS=/proc/interrupts
export INTERRUPT_PREFIX=i2c_omap
export TEMP_FILE_1=/tmp/i2c.test.file.1
export TEMP_FILE_2=/tmp/i2c.test.file.2
export HEAD_OPTION="-n "
export TAIL_OPTION="-n "
export ADAPTER_INFO=None
export MODE=None
export UTILS_DIR_HANDLERS=${TESTROOT}/../../utils/handlers

# Creating local mdev devices
mdev -s

# Checking if we have bc available in our filesystem

if [ ! `echo 1+1 | bc` ]; then
	echo "FATAL: BC is unavailable, cannot continue"
	return 1
fi

# End of file
