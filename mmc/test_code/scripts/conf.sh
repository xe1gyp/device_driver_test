#!/bin/sh

# Testsuite variables
export POSTFIX=`date "+%Y%m%d-%H%M%S"`
export TESTROOT=${PWD}
export TESTBIN=${TESTROOT}/../binaries
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

# External Utilities
. ${TESTROOT}/../../utils/configuration/general.configuration

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


# MMC/SD New Variables

export MMCSD_DEVFS_NAME=mmcblk$SLOT
export MMCSD_DEVFS_ENTRY=/dev/${MMCSD_DEVFS_NAME}
export MMCSD_DEVFS_PARTITION_1=${MMCSD_DEVFS_ENTRY}p1
export MMCSD_DEVFS_PARTITION_2=${MMCSD_DEVFS_ENTRY}p2

export MMCSD_MOUNTPOINT_1=/media/mmcsdp`echo "$SLOT+1" | bc`
export MMCSD_MOUNTPOINT_2=/media/mmcsdp`echo "$SLOT+2" | bc`

export FILE_SIZE_BIG=file.size.big
export FILE_SIZE_SMALL=file.size.small

# Remove any existing partition
$TESTSCRIPT/removePartitions.sh

# See if MMC in inserted, otherwise abort
mount | grep $MMCSD_DEVFS_ENTRY
if [ "$?" -eq "0" ]; then
	echo "FATAL: mmc/sd $MMCSD_DEVFS_ENTRY is not inserted in the specified slot, cannot continue"
	exit 1
fi

# End of file
