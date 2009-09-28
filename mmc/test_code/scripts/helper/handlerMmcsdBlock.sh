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

  MMCSD_MOUNTPOINT_1=$4
  MMCSD_MOUNTPOINT_2=$5

	$MMCSD_DIR_HELPER/handlerMmcsdSetup.sh "create" $LOCAL_PARTITIONS

	if [ "$LOCAL_PARTITIONS" = "1" ]; then

		test -d $MMCSD_MOUNTPOINT_1 || mkdir -p $MMCSD_MOUNTPOINT_1

		if [ "$LOCAL_FILESYSTEM_TYPE" = "ext2" ]; then
			$MMCSD_DIR_BINARIES/mke2fs $MMCSD_DEVFS_PARTITION_1
			mount -t ext2 $MMCSD_DEVFS_PARTITION_1 $MMCSD_MOUNTPOINT_1
		elif [ "$LOCAL_FILESYSTEM_TYPE" = "dos" ]; then
			$MMCSD_DIR_BINARIES/mkdosfs $MMCSD_DEVFS_PARTITION_1
			mount -t vfat $MMCSD_DEVFS_PARTITION_1 $MMCSD_MOUNTPOINT_1
		fi


	elif [ "$LOCAL_PARTITIONS" = "2" ]; then

		test -d $MMCSD_MOUNTPOINT_1 || mkdir -p $MMCSD_MOUNTPOINT_1
		test -d $MMCSD_MOUNTPOINT_2 || mkdir -p $MMCSD_MOUNTPOINT_2

		if [ "$LOCAL_FILESYSTEM_TYPE" = "ext2" ]; then
			$MMCSD_DIR_BINARIES/mke2fs $MMCSD_DEVFS_PARTITION_1
			$MMCSD_DIR_BINARIES/mke2fs $MMCSD_DEVFS_PARTITION_2
			mount -t ext2 $MMCSD_DEVFS_PARTITION_1 $MMCSD_MOUNTPOINT_1
			mount -t ext2 $MMCSD_DEVFS_PARTITION_2 $MMCSD_MOUNTPOINT_2
		elif [ "$LOCAL_FILESYSTEM_TYPE" = "dos" ]; then
			$MMCSD_DIR_BINARIES/mkdosfs $MMCSD_DEVFS_PARTITION_1
			$MMCSD_DIR_BINARIES/mkdosfs $MMCSD_DEVFS_PARTITION_2
			mount -t vfat $MMCSD_DEVFS_PARTITION_1 $MMCSD_MOUNTPOINT_1
			mount -t vfat $MMCSD_DEVFS_PARTITION_2 $MMCSD_MOUNTPOINT_2
    elif [ "$LOCAL_FILESYSTEM_TYPE" = "mixed" ]; then
      $MMCSD_DIR_BINARIES/mke2fs $MMCSD_DEVFS_PARTITION_1
		  mount -t ext2 $MMCSD_DEVFS_PARTITION_1 $MMCSD_MOUNTPOINT_1
		  $MMCSD_DIR_BINARIES/mkdosfs $MMCSD_DEVFS_PARTITION_2
		  mount -t vfat $MMCSD_DEVFS_PARTITION_2 $MMCSD_MOUNTPOINT_2
		fi

	fi
	mount

elif [ "$LOCAL_COMMAND" = "remove" ]; then

  MMCSD_MOUNTPOINT_1=$3
  MMCSD_MOUNTPOINT_2=$4

	if [ "$LOCAL_PARTITIONS" = "1" ]; then
		sync && umount $MMCSD_DEVFS_PARTITION_1
	elif [ "$LOCAL_PARTITIONS" = "2" ]; then
		sync && umount $MMCSD_DEVFS_PARTITION_1
		sync && umount $MMCSD_DEVFS_PARTITION_2
	fi
	mount

  test -d $MMCSD_MOUNTPOINT_1 && rm -rf $MMCSD_MOUNTPOINT_1
  test -d $MMCSD_MOUNTPOINT_2 && rm -rf $MMCSD_MOUNTPOINT_2

	$MMCSD_DIR_HELPER/handlerMmcsdSetup.sh "remove" $LOCAL_PARTITIONS

fi

# End of file
