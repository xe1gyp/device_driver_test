#!/bin/sh

#If we have a working rtc, this is good.. else use next line
export POSTFIX=`date "+%Y%m%d-%H%M%S"`
#export POSTFIX=$$
# Load up the defaults
export TESTROOT=${PWD}
export TESTBIN=${PWD}/../bin
export UTILBIN=${PWD}/../../utils/bin
export TESTMODS=${PWD}/../mods
export TESTSCRIPT=${PWD}/helper
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

#TYPE="OneNAND"
#TYPE="8Bit-NAND"

if [ "$TYPE" == "" ]
then
	echo "FATAL: Please specify NAND type -> OneNAND / NAND / 8Bit-NAND"
	exit 1
fi

#Offset unknow variables
export OFFSET_VAL1=0xA0000
export OFFSET_VAL2=0xc0000
export OFFSET_VAL3=655360
export OFFSET_VAL4=786432

# File system images
export JFFS2_IMAGE=$TESTSCRIPT/onenand_udev.jffs2
export DEFAULT_MOUNT_POINT=/mnt/nand

if [ -f  "$DEFAULT_MOUNT_POINT" ]
then
	mkdir -p $DEFAULT_MOUNT_POINT
fi

# Memory specifications
if [ "$TYPE" = "OneNAND" ]; then
	export JFFS_OPTIONS=""
	export PAGE_SIZE=2048
	export BLOCK_SIZE=131072
	export SECTORS_PER_PAGE=4
	export PAGES_PER_BLOCK=64
elif [ "$TYPE" = "NAND" ]; then
	export JFFS_OPTIONS="-j"
	export PAGE_SIZE=512
	export BLOCK_SIZE=16384
	export PAGES_PER_BLOCK=32
elif [ "$TYPE" = "8Bit-NAND" ]; then
	export JFFS_OPTIONS=""
	export PAGE_SIZE=2048
	export BLOCK_SIZE=131072
	export PAGES_PER_BLOCK=64
else
	echo "The memory type has not being specified is not valid"
	exit 1
fi

if [ "$TYPE" == "OneNAND" ]; then
	
	#export DEV_SIZE=131203072
	export DEV_SIZE=265420800
	
	# Filesystem partition
	MTD_FILE=`cat /proc/mtd | grep OneNAND | grep "File" | cut -d ':' -f0`
	export MTD_CHAR_DEV1=/dev/$MTD_FILE
	MTD_NUMBER=`echo $MTD_FILE | cut -c4-`
	export MTD_BLK_DEV1=/dev/mtdblock${MTD_NUMBER}
	
	# Kernel partition
	MTD_FILE=`cat /proc/mtd | grep OneNAND | grep "Kernel" | cut -d ':' -f0`
	export MTD_CHAR_DEV2=/dev/$MTD_FILE
	MTD_NUMBER=`echo $MTD_FILE | cut -c4-`
	export MTD_BLK_DEV2=/dev/mtdblock${MTD_NUMBER}
	
	#export MTD_CHAR_DEV1=/dev/mtd13
	#export MTD_BLK_DEV1=/dev/mtdblock13
	#export MTD_CHAR_DEV2=/dev/mtd12
	#export MTD_BLK_DEV2=/dev/mtdblock13
	
elif [ "$TYPE" == "NAND" ]; then
	export DEV_SIZE=128712704

	# Filesystem partition
	MTD_FILE=`cat /proc/mtd | grep "File System - NAND" | cut -d ':' -f0`
	export MTD_CHAR_DEV1=/dev/$MTD_FILE
	MTD_NUMBER=`echo $MTD_FILE | cut -c4-`
	export MTD_BLK_DEV1=/dev/mtdblock${MTD_NUMBER}	
	
	# Kernel partition
	MTD_FILE=`cat /proc/mtd | grep "Kernel-NAND" | cut -d ':' -f0`
	export MTD_CHAR_DEV2=/dev/$MTD_FILE
	MTD_NUMBER=`echo $MTD_FILE | cut -c4-`
	export MTD_BLK_DEV2=/dev/mtdblock${MTD_NUMBER}	
	
	#export MTD_CHAR_DEV1=/dev/mtd8
	#export MTD_BLK_DEV1=/dev/mtdblock8
	#export MTD_CHAR_DEV2=/dev/mtd7
	#export MTD_BLK_DEV2=/dev/mtdblock7
	
else
	echo "The memory type specified is not valid"
	exit 1
fi

echo Main Partition: $MTD_CHAR_DEV1
echo Aux Partition : $MTD_CHAR_DEV2
echo sixe=$DEV_SIZE
