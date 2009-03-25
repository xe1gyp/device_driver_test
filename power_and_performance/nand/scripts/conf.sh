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

if [ "$TYPE" == "" ]
then
	echo "FATAL: What NAND Memory will you work with? -> OneNAND / NAND"
	echo "FATAL: Please export TYPE variable to specify"
	exit 1
fi

# Memory specifications
if [ "$TYPE" = "OneNAND" ]; then

	export JFFS_OPTIONS=""

	# Filesystem partition
	MTD_FILE=`cat /proc/mtd | grep OneNAND | grep "File" | cut -d ':' -f0`
	export MTD_CHAR_DEV1=/dev/$MTD_FILE
	MTD_NUMBER=`echo $MTD_FILE | cut -c4-`
	export MTD_BLK_DEV1=/dev/mtdblock${MTD_NUMBER}

	export DEFAULT_MOUNT_POINT=/mnt/onenand

elif [ "$TYPE" = "NAND" ]; then

	export JFFS_OPTIONS="-j"

	# Filesystem partition
	MTD_FILE=`cat /proc/mtd | grep "File System - NAND" | cut -d ':' -f0`
	export MTD_CHAR_DEV1=/dev/$MTD_FILE
	MTD_NUMBER=`echo $MTD_FILE | cut -c4-`
	export MTD_BLK_DEV1=/dev/mtdblock${MTD_NUMBER}

	export DEFAULT_MOUNT_POINT=/mnt/nand

else
	echo "The memory type has not being specified is not valid"
	exit 1
fi

echo Main Partition: $MTD_CHAR_DEV1

# Utilities
export UTILBIN=${TESTROOT}/../../utils/bin
export WAITKEY=$UTILBIN/akey
export BONNIE=$UTILBIN/Bonnie
export IOZONE=$UTILBIN/iozone
export TOP2=$UTILBIN/top2

# Specific Variables
export REPETITIONS=5
export OPERATION=""
export CACHE_SIZE=0
export FILE_SIZE=0
export RECORD_SIZE=0
export SLEEPTIME=15
export CPUUSAGE=$TESTROOT/cpu.usage
export COMMAND=""
export CUNTER=0

# End of file
