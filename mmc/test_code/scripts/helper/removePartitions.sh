#!/bin/sh

RP_MMCSD_DEVFS_ENTRY=$1
RP_MMCSD_DEVFS_PARTITION_1=${RP_MMCSD_DEVFS_ENTRY}p1
RP_MMCSD_DEVFS_PARTITION_2=${RP_MMCSD_DEVFS_ENTRY}p2

DeletePartitions()
{

	NUM_PARTITIONS=$(ls $RP_MMCSD_DEVFS_ENTRY* | wc -l)
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
	        echo -e "$FDISK_STRING" | fdisk $RP_MMCSD_DEVFS_ENTRY
	fi
}

echo "Checking for mmc/sd partitions, if exist then remove them"
mount | grep  $RP_MMCSD_DEVFS_PARTITION_1 && umount `mount | grep $RP_MMCSD_DEVFS_PARTITION_1 | awk '{print $3}'`
mount | grep  $RP_MMCSD_DEVFS_PARTITION_2 && umount `mount | grep $RP_MMCSD_DEVFS_PARTITION_2 | awk '{print $3}'`
DeletePartitions $RP_MMCSD_DEVFS_ENTRY

# End of file
