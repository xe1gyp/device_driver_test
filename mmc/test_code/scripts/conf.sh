#!/bin/sh

GetEndSector(){
	echo -e "p\nq\n" | fdisk $1 | grep "heads" | awk '{print $5}'
}

# MMC device dependent parameters
export DEFAULT_MOUNT_POINT1=/mnt/mmc1
export DEFAULT_MOUNT_POINT2=/mnt/mmc2

# MMC 1
export ROOT_ENTRY_DEV1=/dev/mmcblk0
export PARTITION1_NAME_DEV1=mmcblk0p1
export PARTITION2_NAME_DEV1=mmcblk0p2
export PARTITION1_ENTRY_DEV1=/dev/$PARTITION1_NAME_DEV1
export PARTITION2_ENTRY_DEV1=/dev/$PARTITION2_NAME_DEV1
export MOUNT_POINT1=/mnt/mmc1
export MOUNT_POINT3=/mnt/mmc3
export START_SECTOR_DEV1=1
export END_SECTOR_DEV1=`GetEndSector $ROOT_ENTRY_DEV1`
export MIDDLE_SECTOR_DEV1=`echo $END_SECTOR_DEV1/2|bc`

# MMC 2
export ROOT_ENTRY_DEV2=/dev/mmcblk1
export PARTITION1_NAME_DEV2=mmcblk1p1
export PARTITION2_NAME_DEV2=mmcblk1p2
export PARTITION1_ENTRY_DEV2=/dev/$PARTITION1_NAME_DEV2
export PARTITION2_ENTRY_DEV2=/dev/$PARTITION2_NAME_DEV2
export MOUNT_POINT2=/mnt/mmc2
export MOUNT_POINT4=/mnt/mmc4
export START_SECTOR_DEV2=1
export END_SECTOR_DEV2=`GetEndSector $ROOT_ENTRY_DEV2`
export MIDDLE_SECTOR_DEV2=`echo $END_SECTOR_DEV2/2|bc`

# Testsuite variables
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
export DURATION=""
export PATH="${PATH}:${TESTROOT}:${TESTBIN}:${TESTSCRIPT}"
export TC_SCENARIO="${TESTROOT}/scenarios"
export SCENARIO_NAMES=""

export BIG_FILE=bigfile.tar.bz2
export SMALL_FILES=smallfiles

# Let's remove any existing partition
$TESTSCRIPT/RemovePartitions.bash

# End of file
