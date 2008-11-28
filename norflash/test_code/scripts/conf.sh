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

# NOR default device
export DEFAULT_DEVICE=SIBLEY

# Filesystem partition
MTD_FILE=`cat /proc/mtd | grep NOR | grep "File" | cut -d ':' -f0`
export MTD_CHAR_DEV1=/dev/$MTD_FILE
MTD_NUMBER=`echo $MTD_FILE | cut -c4-`
export MAIN_PARTITION=$MTD_NUMBER
export MTD_BLK_DEV1=/dev/mtdblock${MTD_NUMBER}
	
# Kernel partition
MTD_FILE=`cat /proc/mtd | grep NOR | grep "Kernel" | cut -d ':' -f0`
export MTD_CHAR_DEV2=/dev/$MTD_FILE
MTD_NUMBER=`echo $MTD_FILE | cut -c4-`
export AUX_PARTITION=$MTD_NUMBER
export MTD_BLK_DEV2=/dev/mtdblock${MTD_NUMBER}
	
echo Main Partition: mtd$MAIN_PARTITION
echo Aux Partition : mtd$AUX_PARTITION

# Partitions information
# Strata Flash
export SECTORSIZE_ONE_SIB=262144
export MAX_SIZE_SIB=0x7d80000
export MAX_SIZE_SIB2=0x00200000
export SIBLEY_JFFS_FILE=$TESTSCRIPT/rootfs.sibley.bin.nor
# Sibley Flash
export SECTORSIZE_ONE_STRATA=131072
export MAX_SIZE_STRATA=0x03da0000
export MAX_SIZE_STRATA2=0x00200000
export STRATA_JFFS_FILE=$TESTSCRIPT/rootfs.strata.bin.nor

# Device-dependent parameters
if [ "$DEFAULT_DEVICE" = "STRATA" ]; then
        #sector 0x40000
        export SECTORSIZE_ONE=$SECTORSIZE_ONE_STRATA
        # Max NOR size
        export MAX_SIZE=$MAX_SIZE_STRATA
        export MAX_SIZE2=$MAX_SIZE_STRATA2
        export JFFS_FILE=$BASE/$STRATA_JFFS_FILE
        JFFS_OPTION="-j"
fi

if [ "$DEFAULT_DEVICE" = "SIBLEY" ]; then
        #sector 0x40000
        export SECTORSIZE_ONE=$SECTORSIZE_ONE_SIB
        # Max NOR size
        export MAX_SIZE=$MAX_SIZE_SIB
        export MAX_SIZE2=$MAX_SIZE_SIB2
        export JFFS_FILE=$BASE/$SIBLEY_JFFS_FILE
        export JFFS_OPTION=""
fi
