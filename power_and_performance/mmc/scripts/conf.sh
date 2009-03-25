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

# Check if the mmc card is mounted
#df | grep -e '/mnt/mmc$'

#if [ $? -eq 1 ]; then
#	echo "FATAL: MMC CARD NOT MOUNTED"
#	exit 1
#fi

# MMC device dependent parameters
export DEFAULT_MOUNT_POINT2=/mnt/mmc2

# MMC 2
export ROOT_ENTRY_DEV1=/dev/mmcblk0
export PARTITION2_NAME_DEV1=mmcblk0p2
export PARTITION2_ENTRY_DEV1=/dev/$PARTITION2_NAME_DEV1

# Utilities
export UTILBIN=${TESTROOT}/../../utils/bin
export WAITKEY=$UTILBIN/akey
export BONNIE=$UTILBIN/Bonnie
export IOZONE=$UTILBIN/iozone
export TOP2=$UTILBIN/top2

# Specific Variables
export REPETITIONS=2
export OPERATION=""
export CACHE_SIZE=0
export FILE_SIZE=0
export RECORD_SIZE=0
export SLEEPTIME=15
export CPUUSAGE=$TESTROOT/cpu.usage
export COMMAND=""

# End of file
