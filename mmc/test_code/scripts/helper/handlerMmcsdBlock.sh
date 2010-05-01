#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_OPERATION=$1
LOCAL_PARTITIONS=$2

# =============================================================================
# Functions
# =============================================================================

fstabModifier() {

	LOCAL_MMCSD_FILESYSTEM_TYPE=$1
	LOCAL_MMCSD_DEVFS_PARTITION=$2
	LOCAL_MMCSD_MOUNTPOINT=$3

	echo "${LOCAL_MMCSD_DEVFS_PARTITION} ${LOCAL_MMCSD_MOUNTPOINT} $LOCAL_MMCSD_FILESYSTEM_TYPE defaults 0 0" >> /etc/fstab
	cat /etc/fstab

}

xMountFunction() {

	LOCAL_MMCSD_PARTITION_NUMBER=$1
	LOCAL_MMCSD_FILESYSTEM_TYPE=$2
	LOCAL_MMCSD_DEVFS_PARTITION=$3
	LOCAL_MMCSD_MOUNTPOINT=$4

	# Required for POKY as it AUTO MOUNTS ONCE
	# PARTITON GETS CREATED
	mount | grep $LOCAL_MMCSD_DEVFS_PARTITION && umount $LOCAL_MMCSD_DEVFS_PARTITION

	mount -t $LOCAL_MMCSD_FILESYSTEM_TYPE $LOCAL_MMCSD_DEVFS_PARTITION $LOCAL_MMCSD_MOUNTPOINT
	handlerError.sh "log" "$?" "halt" "handlerMmmcsdBlock"

	fstabModifier $LOCAL_MMCSD_FILESYSTEM_TYPE $LOCAL_MMCSD_DEVFS_PARTITION $LOCAL_MMCSD_MOUNTPOINT

}

# =============================================================================
# Main
# =============================================================================

handlerError.sh "test"
if [ $? -eq 1 ]; then
	return 1
fi

if [ "$LOCAL_OPERATION" = "create" ]; then

	LOCAL_FILESYSTEM_TYPE=$3

	if [ "$LOCAL_FILESYSTEM_TYPE" == "" ]; then
		handlerError.sh "log" "1" "halt" "handlerMmmcsdBlock"
		exit 1
	fi

	if [ ! -z $4 ] ; then
		MMCSD_MOUNTPOINT_1=$4
		MMCSD_MOUNTPOINT_2=$5
	fi

	handlerFstab.sh "save"

	handlerMmcsdSetup.sh "create" $LOCAL_PARTITIONS

	if [ "$LOCAL_PARTITIONS" = "1" ]; then

		test -d $MMCSD_MOUNTPOINT_1 || mkdir -p $MMCSD_MOUNTPOINT_1

		if [ "$LOCAL_FILESYSTEM_TYPE" = "ext2" ]; then

			$MMCSD_DIR_BINARIES/mke2fs $MMCSD_DEVFS_PARTITION_1
			xMountFunction "1" "ext2" $MMCSD_DEVFS_PARTITION_1 $MMCSD_MOUNTPOINT_1

		elif [ "$LOCAL_FILESYSTEM_TYPE" = "dos" ]; then

			$MMCSD_DIR_BINARIES/mkdosfs $MMCSD_DEVFS_PARTITION_1
			xMountFunction "1" "msdos" $MMCSD_DEVFS_PARTITION_1 $MMCSD_MOUNTPOINT_1

		fi


	elif [ "$LOCAL_PARTITIONS" = "2" ]; then

		test -d $MMCSD_MOUNTPOINT_1 || mkdir -p $MMCSD_MOUNTPOINT_1
		test -d $MMCSD_MOUNTPOINT_2 || mkdir -p $MMCSD_MOUNTPOINT_2

		if [ "$LOCAL_FILESYSTEM_TYPE" = "ext2" ]; then

			$MMCSD_DIR_BINARIES/mke2fs $MMCSD_DEVFS_PARTITION_1
			xMountFunction "1" "ext2" $MMCSD_DEVFS_PARTITION_1 $MMCSD_MOUNTPOINT_1

			$MMCSD_DIR_BINARIES/mke2fs $MMCSD_DEVFS_PARTITION_2
			xMountFunction "2" "ext2" $MMCSD_DEVFS_PARTITION_2 $MMCSD_MOUNTPOINT_2

		elif [ "$LOCAL_FILESYSTEM_TYPE" = "dos" ]; then

			$MMCSD_DIR_BINARIES/mkdosfs $MMCSD_DEVFS_PARTITION_1
			xMountFunction "1" "msdos" $MMCSD_DEVFS_PARTITION_1 $MMCSD_MOUNTPOINT_1

			$MMCSD_DIR_BINARIES/mkdosfs $MMCSD_DEVFS_PARTITION_2
			xMountFunction "2" "msdos" $MMCSD_DEVFS_PARTITION_2 $MMCSD_MOUNTPOINT_2

		elif [ "$LOCAL_FILESYSTEM_TYPE" = "mixed" ]; then

			$MMCSD_DIR_BINARIES/mke2fs $MMCSD_DEVFS_PARTITION_1
			xMountFunction "1" "ext2" $MMCSD_DEVFS_PARTITION_1 $MMCSD_MOUNTPOINT_1

			$MMCSD_DIR_BINARIES/mkdosfs $MMCSD_DEVFS_PARTITION_2
			xMountFunction "2" "msdos" $MMCSD_DEVFS_PARTITION_1 $MMCSD_MOUNTPOINT_1

		fi

	fi

	mount


elif [ "$LOCAL_OPERATION" = "remove" ]; then

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

	handlerFstab.sh "restore"


elif [ "$LOCAL_OPERATION" = "remount" ]; then

	if [ "$LOCAL_PARTITIONS" = "1" ]; then

		sync && umount $MMCSD_DEVFS_PARTITION_1
		mount $MMCSD_DEVFS_PARTITION_1
		handlerError.sh "log" "$?" "halt" "handlerMmmcsdBlock"

	elif [ "$LOCAL_PARTITIONS" = "2" ]; then

		sync && umount $MMCSD_DEVFS_PARTITION_1
		sync && umount $MMCSD_DEVFS_PARTITION_2
		mount $MMCSD_DEVFS_PARTITION_1
		handlerError.sh "log" "$?" "halt" "handlerMmmcsdBlock"
		mount $MMCSD_DEVFS_PARTITION_2
		handlerError.sh "log" "$?" "halt" "handlerMmmcsdBlock"

	else

		sync && umount $LOCAL_PARTITIONS
		mount $LOCAL_PARTITIONS
		handlerError.sh "log" "$?" "halt" "handlerMmmcsdBlock"

	fi

	mount

fi

# End of file
