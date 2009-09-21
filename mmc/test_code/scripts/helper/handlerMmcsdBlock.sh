#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_COMMAND=$1
LOCAL_PARTITIONS=$2
LOCAL_FILESYSTEM_TYPE=$3

# =============================================================================
# Functions
# =============================================================================


# =============================================================================
# Main
# =============================================================================

if [ "$LOCAL_COMMAND" = "create" ]; then

	$TESTSCRIPT/handlerMmcsdSetup.sh "create" $LOCAL_PARTITIONS

	if [ "$LOCAL_PARTITIONS" = "1" ]; then

		test -d $MMCSD_MOUNTPOINT_1 || mkdir $MMCSD_MOUNTPOINT_1

		if [ "$LOCAL_FILESYSTEM_TYPE" = "ext2" ]; then
			$TESTBIN/mke2fs $MMCSD_DEVFS_PARTITION_1
			mount -t ext2 $MMCSD_DEVFS_PARTITION_1 $MMCSD_MOUNTPOINT_1
		elif [ "$LOCAL_FILESYSTEM_TYPE" = "dos" ]; then
			$TESTBIN/mkdosfs $MMCSD_DEVFS_PARTITION_1
			mount -t vfat $MMCSD_DEVFS_PARTITION_1 $MMCSD_MOUNTPOINT_1
		fi


	elif [ "$LOCAL_PARTITIONS" = "2" ]; then
		test -d $MMCSD_MOUNTPOINT_1 || mkdir $MMCSD_MOUNTPOINT_1
		test -d $MMCSD_MOUNTPOINT_2 || mkdir $MMCSD_MOUNTPOINT_2

		if [ "$LOCAL_FILESYSTEM_TYPE" = "ext2" ]; then
			$TESTBIN/mke2fs $MMCSD_DEVFS_PARTITION_1
			$TESTBIN/mke2fs $MMCSD_DEVFS_PARTITION_2
			mount -t ext2 $MMCSD_DEVFS_PARTITION_1 $MMCSD_MOUNTPOINT_1
			mount -t ext2 $MMCSD_DEVFS_PARTITION_2 $MMCSD_MOUNTPOINT_2
		elif [ "$LOCAL_FILESYSTEM_TYPE" = "dos" ]; then
			$TESTBIN/mkdosfs $MMCSD_DEVFS_PARTITION_1
			$TESTBIN/mkdosfs $MMCSD_DEVFS_PARTITION_2
			mount -t vfat $MMCSD_DEVFS_PARTITION_1 $MMCSD_MOUNTPOINT_1
			mount -t vfat $MMCSD_DEVFS_PARTITION_2 $MMCSD_MOUNTPOINT_2
		fi

	fi

	mount
	df
	sleep 5

elif [ "$LOCAL_COMMAND" = "remove" ]; then

	if [ "$LOCAL_PARTITIONS" = "1" ]; then
		sync && umount $MMCSD_DEVFS_PARTITION_1
	elif [ "$LOCAL_PARTITIONS" = "2" ]; then
		sync && umount $MMCSD_DEVFS_PARTITION_1
		sync && umount $MMCSD_DEVFS_PARTITION_2
	fi

	$TESTSCRIPT/handlerMmcsdSetup.sh "remove" $LOCAL_PARTITIONS

fi

# End of file
