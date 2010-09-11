#!/bin/sh

# TestSuite General Variables
export MMCSD_POSTFIX=`date "+%Y%m%d-%H%M%S"`
export MMCSD_ROOT=`pwd`

export MMCSD_DIR_BINARIES=${MMCSD_ROOT}/../bin
export MMCSD_DIR_HELPER=${MMCSD_ROOT}/helper
export MMCSD_DIR_TMP=${MMCSD_ROOT}/tmp
export MMCSD_DIR_TEST=${MMCSD_ROOT}/test
export MMCSD_DIR_SCENARIOS="${MMCSD_ROOT}/scenarios"

export MMCSD_FILE_OUTPUT=${MMCSD_ROOT}/output.$MMCSD_POSTFIX
export MMCSD_FILE_LOG=${MMCSD_ROOT}/log.$MMCSD_POSTFIX
export MMCSD_FILE_TMP=${MMCSD_DIR_TMP}/tmp.$MMCSD_POSTFIX
export MMCSD_FILE_CMD=cmd.$MMCSD_POSTFIX

export MMCSD_DURATION=""
export MMCSD_PRETTY_PRT=""
export MMCSD_VERBOSE=""
export MMCSD_SCENARIO_NAMES=""
export MMCSD_STRESS=""
export MMCSD_BLOCK_FOLDER=/dev

export PATH="$PATH:$MMCSD_ROOT:$MMCSD_DIR_HELPER:$MMCSD_DIR_BINARIES"

# Utils General Variables
export UTILS_DIR=$MMCSD_ROOT/../../utils/
export UTILS_DIR_BIN=$UTILS_DIR/bin
export UTILS_DIR_HANDLERS=$UTILS_DIR/handlers
export UTILS_DIR_SCRIPTS=$UTILS_DIR/scripts

. $UTILS_DIR/configuration/general.configuration

export PATH="$PATH:$UTILS_DIR_BIN:$UTILS_DIR_HANDLERS:$UTILS_DIR_SCRIPTS"

# MMC/SD General Variables
if [ "$SLOT" = "" ]
then
	echo "FATAL: Please specify the slot number by exporting it through SLOT variable"
	echo "Available Values: Depend on Development Board, beginning by 0"
	echo "e.g. export SLOT=0"
	exit 1
fi

handlerFilesystem.sh getType
FS_TYPE=`cat $HFS_FS_TYPE`

if [ "$FS_TYPE" = "android" ]; then
	export MMCSD_BLOCK_FOLDER=/dev/block
fi

export MMCSD_DEVFS_NAME=mmcblk$SLOT
export MMCSD_DEVFS_ENTRY=$MMCSD_BLOCK_FOLDER/${MMCSD_DEVFS_NAME}
export MMCSD_DEVFS_PARTITION_1=${MMCSD_DEVFS_ENTRY}p1
export MMCSD_DEVFS_PARTITION_2=${MMCSD_DEVFS_ENTRY}p2

export MMCSD_MOUNTPOINT_1_LOG=$MMCSD_DIR_TMP/mmcsd.mountpoint.1
export MMCSD_MOUNTPOINT_2_LOG=$MMCSD_DIR_TMP/mmcsd.mountpoint.2
export MMCSD_MOUNTPOINT_1=/media/mmcsdp`echo "$SLOT+1" | bc`
export MMCSD_MOUNTPOINT_2=/media/mmcsdp`echo "$SLOT+2" | bc`

if [ "$SLOT" = "0" ]; then
	export MMC_IRQ_NUM=118
elif [ "$SLOT" = "1" ]; then
	export MMC_IRQ_NUM=115
fi

export MMCSD_TMPFS_MOUNTPOINT=/media/tmpfs

export MMCSD_FILE_SIZE_BIG=file.size.big
export MMCSD_FILE_SIZE_SMALL=file.size.small

# System Variables
export PROCFS_DEVICES=/proc/devices
export PROCFS_INTERRUPTS=/proc/interrupts
export PROCFS_PARTITIONS=/proc/partitions
export SYS_MMC_HOST=/sys/class/mmc_host

# Check if bc is available, otherwise abort
if [ ! `echo 1+1 | bc` ]; then
	echo "FATAL: BC is unavailable, cannot continue"
	exit 1
fi

# Remove any error file
handlerError.sh "clean"

# End of file
