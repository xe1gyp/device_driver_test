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

	LOCAL_MMCSD_PARTITION_NUMBER=$1
	LOCAL_MMCSD_FILESYSTEM_TYPE=$2
	LOCAL_MMCSD_DEVFS_PARTITION=$3
	LOCAL_MMCSD_MOUNTPOINT=$4

	handlerFstab.sh "add" "$LOCAL_MMCSD_PARTITION_NUMBER" "${LOCAL_MMCSD_DEVFS_PARTITION} ${LOCAL_MMCSD_MOUNTPOINT} $LOCAL_MMCSD_FILESYSTEM_TYPE defaults 0 0"
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
	if [ "$?" -ne "0" ]; then
		echo "FATAL: Unknown $LOCAL_MMCSD_FILESYSTEM_TYPE filesystem type"
		handlerError.sh "log" "1" "halt" "handlerMmmcsdBlock"
		exit 1
	fi

	handlerError.sh "log" "$?" "halt" "handlerMmmcsdBlock"
	# Ubuntu has an issue were if any scenario forced to quit will make the ubuntu not boot as fstan entry is present
	# fstabModifier $LOCAL_MMCSD_PARTITION_NUMBER $LOCAL_MMCSD_FILESYSTEM_TYPE $LOCAL_MMCSD_DEVFS_PARTITION $LOCAL_MMCSD_MOUNTPOINT

}

partitionFormatHelper() {

	LOCAL_FORMAT_APPLICATION=$1
	LOCAL_MMCSD_MOUNTPOINT_1=$2
	LOCAL_MMCSD_MOUNTPOINT_2=$3

	eval $LOCAL_FORMAT_APPLICATION
	if [ "$?" -eq "127" ]; then
		echo "FATAL: application $LOCAL_FORMAT_APPLICATION is not available un current filesystem"
		handlerError.sh "log" "1" "halt" "handlerMmmcsdBlock"
		exit 1
	fi

	if [ ! -z $2 ] ; then

		$LOCAL_FORMAT_APPLICATION $MMCSD_DEVFS_PARTITION_1
		xMountFunction "1" $LOCAL_FILESYSTEM_TYPE $MMCSD_DEVFS_PARTITION_1 $LOCAL_MMCSD_MOUNTPOINT_1
			
	fi


	if [ ! -z $3 ] ; then

		$LOCAL_FORMAT_APPLICATION $MMCSD_DEVFS_PARTITION_2
		xMountFunction "2" $LOCAL_FILESYSTEM_TYPE $MMCSD_DEVFS_PARTITION_2 $LOCAL_MMCSD_MOUNTPOINT_2
			
	fi

}

# =============================================================================
# Main
# =============================================================================

if [ "$LOCAL_OPERATION" = "create" ]; then

	LOCAL_FILESYSTEM_TYPE=$3

	if [ "$LOCAL_FILESYSTEM_TYPE" = "" ]; then
		handlerError.sh "log" "1" "halt" "handlerMmmcsdBlock"
		exit 1
	fi

	if [ ! -z $4 ] ; then
		MMCSD_MOUNTPOINT_1=$4
		echo "$MMCSD_MOUNTPOINT_1" > $MMCSD_MOUNTPOINT_1_LOG
		MMCSD_MOUNTPOINT_2=$5
		echo "$MMCSD_MOUNTPOINT_2" > $MMCSD_MOUNTPOINT_2_LOG
	fi

	handlerFstab.sh "save"

	handlerMmcsdSetup.sh "create" $LOCAL_PARTITIONS

	if [ "$LOCAL_PARTITIONS" = "1" ]; then

		mount | grep $MMCSD_DEVFS_PARTITION_1 && umount $MMCSD_DEVFS_PARTITION_1

		test -d $MMCSD_MOUNTPOINT_1 || mkdir -p $MMCSD_MOUNTPOINT_1

		if [ "$LOCAL_FILESYSTEM_TYPE" = "ext2" ]; then

			partitionFormatHelper $MMCSD_DIR_BINARIES/mke2fs $MMCSD_MOUNTPOINT_1

		elif [ "$LOCAL_FILESYSTEM_TYPE" = "ext3" ]; then

			partitionFormatHelper mkfs.ext3 $MMCSD_MOUNTPOINT_1

		elif [ "$LOCAL_FILESYSTEM_TYPE" = "ext4" ]; then

			partitionFormatHelper mkfs.ext4 $MMCSD_MOUNTPOINT_1

		elif [ "$LOCAL_FILESYSTEM_TYPE" = "dos" ]; then

			export LOCAL_FILESYSTEM_TYPE=msdos
			partitionFormatHelper $MMCSD_DIR_BINARIES/mkdosfs $MMCSD_MOUNTPOINT_1

		elif [ "$LOCAL_FILESYSTEM_TYPE" = "vfat" ]; then

			partitionFormatHelper mkfs.vfat $MMCSD_MOUNTPOINT_1

		fi


	elif [ "$LOCAL_PARTITIONS" = "2" ]; then


		mount | grep $MMCSD_DEVFS_PARTITION_1 && umount $MMCSD_DEVFS_PARTITION_1
		mount | grep $MMCSD_DEVFS_PARTITION_2 && umount $MMCSD_DEVFS_PARTITION_2

		test -d $MMCSD_MOUNTPOINT_1 || mkdir -p $MMCSD_MOUNTPOINT_1
		test -d $MMCSD_MOUNTPOINT_2 || mkdir -p $MMCSD_MOUNTPOINT_2

		if [ "$LOCAL_FILESYSTEM_TYPE" = "ext2" ]; then

			partitionFormatHelper mkfs.ext2 $MMCSD_MOUNTPOINT_1 $MMCSD_MOUNTPOINT_2

		elif [ "$LOCAL_FILESYSTEM_TYPE" = "ext3" ]; then

			partitionFormatHelper mkfs.ext3 $MMCSD_MOUNTPOINT_1 $MMCSD_MOUNTPOINT_2

		elif [ "$LOCAL_FILESYSTEM_TYPE" = "ext4" ]; then

			partitionFormatHelper mkfs.ext4 $MMCSD_MOUNTPOINT_1 $MMCSD_MOUNTPOINT_2

		elif [ "$LOCAL_FILESYSTEM_TYPE" = "dos" ]; then

			export LOCAL_FILESYSTEM_TYPE=msdos
			partitionFormatHelper $MMCSD_DIR_BINARIES/mkdosfs $MMCSD_MOUNTPOINT_1 $MMCSD_MOUNTPOINT_2

		elif [ "$LOCAL_FILESYSTEM_TYPE" = "vfat" ]; then

			partitionFormatHelper mkfs.vfat $MMCSD_MOUNTPOINT_1 $MMCSD_MOUNTPOINT_2

		elif [ "$LOCAL_FILESYSTEM_TYPE" = "dos-ext2" ]; then

			export LOCAL_FILESYSTEM_TYPE=msdos
			partitionFormatHelper $MMCSD_DIR_BINARIES/mkdosfs $MMCSD_MOUNTPOINT_1
			export LOCAL_FILESYSTEM_TYPE=ext2
			partitionFormatHelper $MMCSD_DIR_BINARIES/mke2fs "" $MMCSD_MOUNTPOINT_2

		elif [ "$LOCAL_FILESYSTEM_TYPE" = "mixed" ]; then

			export LOCAL_FILESYSTEM_TYPE=ext2
			partitionFormatHelper $MMCSD_DIR_BINARIES/mke2fs $MMCSD_MOUNTPOINT_1
			export LOCAL_FILESYSTEM_TYPE=msdos
			partitionFormatHelper $MMCSD_DIR_BINARIES/mkdosfs "" $MMCSD_MOUNTPOINT_2

		fi

	fi

elif [ "$LOCAL_OPERATION" = "remove" ]; then

	if [ ! -z $4 ] ; then

		MMCSD_MOUNTPOINT_1=`cat $MMCSD_MOUNTPOINT_1_LOG`
		MMCSD_MOUNTPOINT_2=`cat $MMCSD_MOUNTPOINT_2_LOG`

	fi

	if [ "$LOCAL_PARTITIONS" = "1" ]; then

		sync && umount $MMCSD_MOUNTPOINT_1

	elif [ "$LOCAL_PARTITIONS" = "2" ]; then

		sync && umount $MMCSD_MOUNTPOINT_1
		sync && umount $MMCSD_MOUNTPOINT_2

	fi

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

	fi

elif [ "$LOCAL_OPERATION" = "fs-test" ]; then

	if [ "$LOCAL_PARTITIONS" = "2" ]; then
		export MMCSD_MOUNTPOINT_1=$MMCSD_MOUNTPOINT_2
	fi
	
	handlerTmpfs.sh "create" "250" $MMCSD_TMPFS_MOUNTPOINT

	handlerCmd.sh "run" "dd if=/dev/urandom of=$MMCSD_TMPFS_MOUNTPOINT/1k.txt bs=1024 count=1"
	handlerCmd.sh "run" "dd if=/dev/urandom of=$MMCSD_TMPFS_MOUNTPOINT/50k.txt bs=1024 count=50"
	handlerCmd.sh "run" "dd if=/dev/urandom of=$MMCSD_TMPFS_MOUNTPOINT/75k.txt bs=1024 count=75"
	handlerCmd.sh "run" "dd if=/dev/urandom of=$MMCSD_TMPFS_MOUNTPOINT/99k.txt bs=1024 count=99"
	handlerCmd.sh "run" "dd if=/dev/urandom of=$MMCSD_TMPFS_MOUNTPOINT/2k.txt bs=1024 count=2"

	handlerCmd.sh "run" "cp $MMCSD_TMPFS_MOUNTPOINT/1k.txt $MMCSD_MOUNTPOINT_1"
	handlerCmd.sh "run" "cp $MMCSD_TMPFS_MOUNTPOINT/50k.txt $MMCSD_MOUNTPOINT_1"
	handlerCmd.sh "run" "cp $MMCSD_TMPFS_MOUNTPOINT/75k.txt $MMCSD_MOUNTPOINT_1"
	handlerCmd.sh "run" "cp $MMCSD_TMPFS_MOUNTPOINT/99k.txt $MMCSD_MOUNTPOINT_1"
	handlerCmd.sh "run" "cp $MMCSD_TMPFS_MOUNTPOINT/2k.txt $MMCSD_MOUNTPOINT_1"

	sync

	handlerCmd.sh "run" "cmp $MMCSD_TMPFS_MOUNTPOINT/1k.txt $MMCSD_MOUNTPOINT_1/1k.txt"
	handlerCmd.sh "run" "cmp $MMCSD_TMPFS_MOUNTPOINT/50k.txt $MMCSD_MOUNTPOINT_1/50k.txt"
	handlerCmd.sh "run" "cmp $MMCSD_TMPFS_MOUNTPOINT/75k.txt $MMCSD_MOUNTPOINT_1/75k.txt"
	handlerCmd.sh "run" "cmp $MMCSD_TMPFS_MOUNTPOINT/99k.txt $MMCSD_MOUNTPOINT_1/99k.txt"
	handlerCmd.sh "run" "cmp $MMCSD_TMPFS_MOUNTPOINT/2k.txt $MMCSD_MOUNTPOINT_1/2k.txt"

	handlerTmpfs.sh "remove" $MMCSD_TMPFS_MOUNTPOINT

fi

# End of file
