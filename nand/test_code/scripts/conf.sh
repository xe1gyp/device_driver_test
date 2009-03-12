#!/bin/sh

export POSTFIX=`date "+%Y%m%d-%H%M%S"`
export TESTROOT=${PWD}
export TESTBIN=${TESTROOT}/../bin
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
export TEMP_SIZE=""
export TEMP_ERASESIZE=""


if [ "$TYPE" == "" ]
then
	echo "FATAL: Please specify NAND type by exporting it through TYPE variable"
	echo "Available Values: OneNAND / NAND"
	echo "e.g. export TYPE=NAND"
	exit 1
fi

#Offset unknow variables
export OFFSET_VAL1=0xA0000
export OFFSET_VAL2=0xc0000
export OFFSET_VAL3=655360
export OFFSET_VAL4=786432

# File system images
#export JFFS2_IMAGE=$TESTSCRIPT/onenand_udev.jffs2
export DEFAULT_MOUNT_POINT=/mnt/flashdevice
export JFFS2_IMAGE=$TESTSCRIPT/onenand_udev.jffs2
export TEMP_FILE=$TMPBASE/tmp.file
export SUMTOOL_FILE=$TESTBIN/sumtool
export DEVFS_URANDOM=/dev/urandom
export DEVFS_ZERO=/dev/zero

if [ -f  "$DEFAULT_MOUNT_POINT" ]
then
	mkdir -p $DEFAULT_MOUNT_POINT
fi

# Memory specifications
if [ "$TYPE" == "OneNAND" ]; then
	
	# Filesystem partition
	export MTD_PROC_ENTRY=`cat /proc/mtd | grep OneNAND | grep "File"`
	MTD_FILE=`echo $MTD_PROC_ENTRY | cut -d ':' -f0`
	export MTD_CHAR_DEV1=/dev/$MTD_FILE
	MTD_NUMBER=`echo $MTD_FILE | cut -c4-`
	export MTD_BLK_DEV1=/dev/mtdblock${MTD_NUMBER}
  export MTD_DEV1_SIZE=`${TESTSCRIPT}/GetDeviceSize.sh "$TEMP_DEVICE"`
  export MTD_DEV1_ERASESIZE=`${TESTSCRIPT}/GetDeviceEraseSize.sh "$TEMP_DEVICE"`
	
	# Kernel partition
	export MTD_PROC_ENTRY=`cat /proc/mtd | grep OneNAND | grep "Kernel"`
	MTD_FILE=`echo $MTD_PROC_ENTRY | cut -d ':' -f0`
	export MTD_CHAR_DEV2=/dev/$MTD_FILE
	MTD_NUMBER=`echo $MTD_FILE | cut -c4-`
	export MTD_BLK_DEV2=/dev/mtdblock${MTD_NUMBER}
	export MTD_DEV2_SIZE=`${TESTSCRIPT}/GetDeviceSize.sh "$TEMP_DEVICE"`
	export MTD_DEV2_ERASESIZE=`${TESTSCRIPT}/GetDeviceEraseSize.sh "$TEMP_DEVICE"`

	export JFFS_OPTIONS=""
	export PAGE_SIZE=2048
	export BLOCK_SIZE=131072
	export SECTORS_PER_PAGE=4
	export PAGES_PER_BLOCK=64


elif [ "$TYPE" == "NAND" ]; then

  # Filesystem partition
	export MTD_PROC_ENTRY=`cat /proc/mtd | grep "File System - NAND"`
	MTD_FILE=`echo $MTD_PROC_ENTRY | cut -d ':' -f0`
	export MTD_CHAR_DEV1=/dev/$MTD_FILE
	MTD_NUMBER=`echo $MTD_FILE | cut -c4-`
	export MTD_BLK_DEV1=/dev/mtdblock${MTD_NUMBER}
	export MTD_DEV1_SIZE=`${TESTSCRIPT}/GetDeviceSize.sh "$TEMP_DEVICE"`
	export MTD_DEV1_ERASESIZE=`${TESTSCRIPT}/GetDeviceEraseSize.sh "$TEMP_DEVICE"`
	
	# Kernel partition
	export MTD_PROC_ENTRY=`cat /proc/mtd | grep "Kernel-NAND"`
	MTD_FILE=`echo $MTD_PROC_ENTRY | cut -d ':' -f0`
	export MTD_CHAR_DEV2=/dev/$MTD_FILE
	MTD_NUMBER=`echo $MTD_FILE | cut -c4-`
	export MTD_BLK_DEV2=/dev/mtdblock${MTD_NUMBER}
	export MTD_DEV2_SIZE=`${TESTSCRIPT}/GetDeviceSize.sh "$TEMP_DEVICE"`
	export MTD_DEV2_ERASESIZE=`${TESTSCRIPT}/GetDeviceEraseSize.sh "$TEMP_DEVICE"`
	
	export JFFS_OPTIONS="-j"
	export PAGE_SIZE=512
	export BLOCK_SIZE=16384
	export PAGES_PER_BLOCK=32

elif [ "$TYPE" = "8Bit-NAND" ]; then

        export JFFS_OPTIONS="-j"
        export PAGE_SIZE=2048
        export BLOCK_SIZE=131072
        export PAGES_PER_BLOCK=64
	
else
	echo "The memory type specified is not valid"
	exit 1
fi

echo Main Partition: $MTD_CHAR_DEV1
echo Main Block Partition $MTD_BLK_DEV1
echo Aux Partition: $MTD_CHAR_DEV2
echo Aux Block Partition: $MTD_BLK_DEV2

test -c $MTD_CHAR_DEV1 && test -b $MTD_BLK_DEV1 || exit 1
test -c $MTD_CHAR_DEV2 && test -b $MTD_BLK_DEV2 || exit 1

# End of file
