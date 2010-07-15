#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_COMMAND=$1
LOCAL_PARTITIONS=$2

# =============================================================================
# Functions
# =============================================================================

GetEndSector(){
	echo -e "c\nu\np\nq\n" | fdisk $1 | grep "heads" | awk '{print $5}' | awk '{print $1}'
}

# =============================================================================
# Main
# =============================================================================

if [ "$LOCAL_COMMAND" = "create" ]; then

	export MMCSD_SECTOR_START=100
	export MMCSD_SECTOR_END=`GetEndSector $MMCSD_DEVFS_ENTRY`
	export MMCSD_SECTOR_MIDDLE=`echo "$MMCSD_SECTOR_END/2" | bc`
	export MMCSD_SECTOR2_START=`echo "$MMCSD_SECTOR_MIDDLE+1" | bc`

	if [ "$LOCAL_PARTITIONS" = "1" ]; then

		echo "Command: p\np\nn\np\n1\n${MMCSD_SECTOR_START}\n${MMCSD_SECTOR_END}\nw\n | fdisk $MMCSD_DEVFS_ENTRY"
		echo -e "p\np\nn\np\n1\n${MMCSD_SECTOR_START}\n${MMCSD_SECTOR_END}\nw\n" | fdisk $MMCSD_DEVFS_ENTRY
		mount | grep $MMCSD_DEVFS_PARTITION_1 && umount $MMCSD_DEVFS_PARTITION_1

	elif [ "$LOCAL_PARTITIONS" = "2" ]; then

		echo "Command: p\np\nn\np\n1\n$MMC_SECTOR_START\n$MMC_SECTOR_MIDDLE\nn\np\n2\n$(echo $MMC_SECTOR_MIDDLE+1|bc)\n$MMC_SECTOR_END\nw\n | fdisk $MMCSD_DEVFS_ENTRY"
		echo -e "p\np\nn\np\n1\n${MMCSD_SECTOR_START}\n${MMCSD_SECTOR_MIDDLE}\nn\np\n2\n${MMCSD_SECTOR2_START}\n${MMCSD_SECTOR_END}\nw\n" | fdisk $MMCSD_DEVFS_ENTRY

		mount | grep $MMCSD_DEVFS_PARTITION_1 && umount $MMCSD_DEVFS_PARTITION_1
		mount | grep $MMCSD_DEVFS_PARTITION_2 && umount $MMCSD_DEVFS_PARTITION_2

	fi

	mount

elif [ "$LOCAL_COMMAND" = "remove" ]; then
	if [ "$LOCAL_PARTITIONS" = "1" ]; then

		mount | grep $MMCSD_DEVFS_ENTRY && umount $MMCSD_DEVFS_ENTRY* | awk '{print $3}'
		echo "Command: p\np\nd\nw\n | fdisk $MMCSD_DEVFS_ENTRY"
		echo -e "p\np\nd\nw\n" | fdisk $MMCSD_DEVFS_ENTRY

	elif [ "$LOCAL_PARTITIONS" = "2" ]; then

		mount | grep $MMCSD_DEVFS_ENTRY && umount $MMCSD_DEVFS_ENTRY* | awk '{print $3}'
		echo "Command: p\np\nd\n1\nd\nw\n | fdisk $MMCSD_DEVFS_ENTRY"
		echo -e "p\np\nd\n1\nd\nw\n" | fdisk $MMCSD_DEVFS_ENTRY

	fi
else
	echo -e "\nError: Cannot recognized command ${LOCAL_COMMAND} under handlerMmcsdSetup.sh\n"
	exit 1
fi

# End of file
