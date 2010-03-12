#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_COMMAND=$1
LOCAL_PARTITIONS=$2

# =============================================================================
# Functions
# =============================================================================


# =============================================================================
# Main
# =============================================================================

handlerError.sh "test"
if [ $? -eq 1 ]; then
	return 1
fi

if [ "$LOCAL_COMMAND" = "create" ]; then

	LOCAL_FILESYSTEM_TYPE=$3

	if [ ! -z $4 ] ; then
		MMCSD_MOUNTPOINT_1=$4
		MMCSD_MOUNTPOINT_2=$5
	fi

	$MMCSD_DIR_HELPER/handlerMmcsdSetup.sh "create" $LOCAL_PARTITIONS

	if [ "$LOCAL_PARTITIONS" = "1" ]; then

		test -d $MMCSD_MOUNTPOINT_1 || mkdir -p $MMCSD_MOUNTPOINT_1

		if [ "$LOCAL_FILESYSTEM_TYPE" = "ext2" ]; then

			$MMCSD_DIR_BINARIES/mke2fs $MMCSD_DEVFS_PARTITION_1
			mount -t ext2 $MMCSD_DEVFS_PARTITION_1 $MMCSD_MOUNTPOINT_1
			handlerError.sh "log" "$?" "halt" "handlerMmmcsdBlock"

		elif [ "$LOCAL_FILESYSTEM_TYPE" = "dos" ]; then

			$MMCSD_DIR_BINARIES/mkdosfs $MMCSD_DEVFS_PARTITION_1
			mount -t msdos $MMCSD_DEVFS_PARTITION_1 $MMCSD_MOUNTPOINT_1
			handlerError.sh "log" "$?" "halt" "handlerMmmcsdBlock"

		fi


	elif [ "$LOCAL_PARTITIONS" = "2" ]; then

		test -d $MMCSD_MOUNTPOINT_1 || mkdir -p $MMCSD_MOUNTPOINT_1
		test -d $MMCSD_MOUNTPOINT_2 || mkdir -p $MMCSD_MOUNTPOINT_2

		if [ "$LOCAL_FILESYSTEM_TYPE" = "ext2" ]; then

			$MMCSD_DIR_BINARIES/mke2fs $MMCSD_DEVFS_PARTITION_1
			mount -t ext2 $MMCSD_DEVFS_PARTITION_1 $MMCSD_MOUNTPOINT_1
			handlerError.sh "log" "$?" "halt" "handlerMmmcsdBlock"

			$MMCSD_DIR_BINARIES/mke2fs $MMCSD_DEVFS_PARTITION_2
			mount -t ext2 $MMCSD_DEVFS_PARTITION_2 $MMCSD_MOUNTPOINT_2
			handlerError.sh "log" "$?" "halt" "handlerMmmcsdBlock"

		elif [ "$LOCAL_FILESYSTEM_TYPE" = "dos" ]; then

			$MMCSD_DIR_BINARIES/mkdosfs $MMCSD_DEVFS_PARTITION_1
			mount -t msdos $MMCSD_DEVFS_PARTITION_1 $MMCSD_MOUNTPOINT_1
			handlerError.sh "log" "$?" "halt" "handlerMmmcsdBlock"

			$MMCSD_DIR_BINARIES/mkdosfs $MMCSD_DEVFS_PARTITION_2
			mount -t msdos $MMCSD_DEVFS_PARTITION_2 $MMCSD_MOUNTPOINT_2
			handlerError.sh "log" "$?" "halt" "handlerMmmcsdBlock"

		elif [ "$LOCAL_FILESYSTEM_TYPE" = "mixed" ]; then

			$MMCSD_DIR_BINARIES/mke2fs $MMCSD_DEVFS_PARTITION_1
			mount -t ext2 $MMCSD_DEVFS_PARTITION_1 $MMCSD_MOUNTPOINT_1
			handlerError.sh "log" "$?" "halt" "handlerMmmcsdBlock"

			$MMCSD_DIR_BINARIES/mkdosfs $MMCSD_DEVFS_PARTITION_2
			mount -t msdos $MMCSD_DEVFS_PARTITION_2 $MMCSD_MOUNTPOINT_2
			handlerError.sh "log" "$?" "halt" "handlerMmmcsdBlock"

		fi

	fi

	mount

elif [ "$LOCAL_COMMAND" = "remove" ]; then

	if [ ! -z $4 ] ; then

		MMCSD_MOUNTPOINT_1=$3
		MMCSD_MOUNTPOINT_2=$4

	fi

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
