#!/bin/sh
# MMC 1
export ROOT_ENTRY_DEV1=/dev/mmcblk0
export PARTITION1_ENTRY_DEV1=/dev/mmcblk0p1
export PARTITION2_ENTRY_DEV1=/dev/mmcblk0p2
export PARTITION1_NAME_DEV1=mmcblk0p1
export PARTITION2_NAME_DEV1=mmcblk0p2
export MOUNT_POINT1=/mnt/mmc1
export MOUNT_POINT3=/mnt/mmc3
export START_SECTOR_DEV1=1

# MMC 2
export ROOT_ENTRY_DEV2=/dev/mmcblk1
export PARTITION1_ENTRY_DEV2=/dev/mmcblk1p1
export PARTITION2_ENTRY_DEV2=/dev/mmcblk1p2
export PARTITION1_NAME_DEV2=mmcblk1p1
export PARTITION2_NAME_DEV2=mmcblk1p2
export MOUNT_POINT2=/mnt/mmc2
export MOUNT_POINT4=/mnt/mmc4
export START_SECTOR_DEV2=1

DeletePartitions()
{
	ROOT_ENTRY_DEVICE=$1
	NUM_PARTITIONS=$(ls $ROOT_ENTRY_DEVICE* | wc -l)
	NUM_PARTITIONS=$(echo $NUM_PARTITIONS-1|bc)

	if [ "$NUM_PARTITIONS" -gt 0 ]; then
		FDISK_STRING="p\n"
		while [ "$NUM_PARTITIONS" -gt 0 ]; do
			if [ "$NUM_PARTITIONS" != 1 ]; then
				NUM="$NUM_PARTITIONS\n"
			else
				NUM=""
			fi
			FDISK_STRING="$FDISK_STRING d\n $NUM"
			NUM_PARTITIONS=$(echo $NUM_PARTITIONS-1|bc)
		done
		FDISK_STRING="$FDISK_STRING w\n"
	        echo -e "$FDISK_STRING" | fdisk $ROOT_ENTRY_DEVICE
	fi
}

echo "Checking for MMC partitions currently mounted"

df | grep "$PARTITION1_ENTRY_DEV1" && echo "$PARTITION1_ENTRY_DEV1 is mounted on $MOUNT_POINT1" && echo "Unmounting..." && umount $MOUNT_POINT1
df | grep "$PARTITION2_ENTRY_DEV1" && echo "$PARTITION2_ENTRY_DEV1 is mounted on $MOUNT_POINT3" && echo "Unmounting..." && umount $MOUNT_POINT3
df | grep "$PARTITION1_ENTRY_DEV2" && echo "$PARTITION1_ENTRY_DEV2 is mounted on $MOUNT_POINT2" && echo "Unmounting..." && umount $MOUNT_POINT2
df | grep "$PARTITION2_ENTRY_DEV2" && echo "$PARTITION2_ENTRY_DEV2 is mounted on $MOUNT_POINT4" && echo "Unmounting..." && umount $MOUNT_POINT4

echo "Removing partitions from MMC devices"

DeletePartitions $ROOT_ENTRY_DEV1
DeletePartitions $ROOT_ENTRY_DEV2
