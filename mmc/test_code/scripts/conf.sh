#!/bin/sh

GetEndSector(){
	echo -e "p\nq\n" | fdisk $1 | grep "heads" | awk '{print $5}'
}

# Testsuite variables
export POSTFIX=`date "+%Y%m%d-%H%M%S"`
export TESTROOT=${PWD}
export TESTBIN=${TESTROOT}/../bin
export UTILBIN=${TESTROOT}/../../utils/bin
export TESTMODS=${TESTROOT}/../mods
export TESTSCRIPT=${TESTROOT}/helper
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
export DEFAULT_MOUNT_POINT=/sdcard
export BLOCK_FOLDER=/dev/block

if [ ! -d "$BLOCK_FOLDER" ]
then
	export BLOCK_FOLDER=/dev
fi



# Check if bc is available, otherwise abort
if [ ! `echo 1+1 | bc` ]; then
	echo "FATAL: BC is unavailable, cannot continue"
	return 1
fi

# Check if SLOT variable is initialized, otherwise abort
if [ "$SLOT" == "" ]
then
	echo "FATAL: Please specify the slot number by exporting it through SLOT variable"
	echo "Available Values: Depend on Development Board, beginning by 0"
	echo "e.g. export SLOT=0"
	exit 1
fi


# MMC specific variables
export PROCFS_DEVICES=/proc/devices
export PROCFS_INTERRUPTS=/proc/interrupts
export PROCFS_PARTITIONS=/proc/partitions

export MMC_ROOT_NAME=mmcblk$SLOT
export MMC_ROOT_ENTRY=$BLOCK_FOLDER/$MMC_ROOT_NAME
export MMC_PARTITION_NAME1=${MMC_ROOT_NAME}p1
export MMC_PARTITION_NAME2=${MMC_ROOT_NAME}p2
export MMC_PARTITION_ENTRY1=$BLOCK_FOLDER/$MMC_PARTITION_NAME1
export MMC_PARTITION_ENTRY2=$BLOCK_FOLDER/$MMC_PARTITION_NAME2
export MMC_MOUNT_POINT_1=/mnt/mmc$SLOT
export MMC_MOUNT_POINT_2=/mnt/mmc`echo "$SLOT+2" | bc`
export MMC_DEVICE_SECTOR_START=1
export MMC_DEVICE_SECTOR_END=`GetEndSector $MMC_ROOT_ENTRY`
export MMC_DEVICE_SECTOR_MIDDLE=`echo $MMC_DEVICE_SECTOR_END/2|bc`

export FILE_SIZE_BIG=bigfile.tar.bz2
export FILE_SIZE_SMALL=smallfiles

# Remove any existing partition
$TESTSCRIPT/RemovePartitions.bash

# See if MMC in inserted, otherwise abort
cat $PROCFS_PARTITIONS | grep $MMC_ROOT_NAME
if [ "$?" -eq "1" ]; then
	echo "FATAL: MMC is not inserted in the specified slot, cannot continue"
	exit 1
fi

# End of file
