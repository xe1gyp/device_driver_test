#!/bin/sh

# TestSuite General Variables
export MMCSDP_POSTFIX=`date "+%Y%m%d-%H%M%S"`
export MMCSDP_ROOT=`pwd`

export MMCSDP_DIR_BINARIES=${MMCSDP_ROOT}/../bin
export MMCSDP_DIR_HELPER=${MMCSDP_ROOT}/helper
export MMCSDP_DIR_TMP=${MMCSDP_ROOT}/tmp
export MMCSDP_DIR_TEST=${MMCSDP_ROOT}/test
export MMCSDP_DIR_SCENARIOS="${MMCSDP_ROOT}/scenarios"

export MMCSDP_FILE_OUTPUT=${MMCSDP_ROOT}/output.$MMCSDP_POSTFIX
export MMCSDP_FILE_LOG=${MMCSDP_ROOT}/log.$MMCSDP_POSTFIX
export MMCSDP_FILE_TMP=${MMCSDP_DIR_TMP}/tmp.$MMCSDP_POSTFIX
export MMCSDP_FILE_CMD=cmd.$MMCSDP_POSTFIX

export MMCSDP_DURATION=""
export MMCSDP_PRETTY_PRT=""
export MMCSDP_VERBOSE=""
export MMCSDP_SCENARIO_NAMES=""
export MMCSDP_STRESS=""
export MMCSDP_BLOCK_FOLDER=/dev/block
export MMCSDP_AUTO_PARTITION=0

export PATH="$PATH:$MMCSDP_ROOT:$MMCSDP_DIR_HELPER:MMCSDP_DIR_BINARIES"

cd ${MMCSDP_ROOT}/../../../mmc/scripts/
. ${MMCSDP_ROOT}/../../../mmc/scripts/conf.sh

# MMC/SD General Variables
if [ "$FORMAT" == "dos" ]; then

	export MMCSDP_AUTO_PARTITION=1
	echo "INFO: Formatting device type dos"

elif [ "$FORMAT" == "vfat" ]; then

	export MMCSDP_AUTO_PARTITION=0
	echo "INFO: Device type vfat"
	cat /etc/fstab | grep vfat
	if [ $? -eq 1 ]; then
		echo "FATAL: Please add vfat entry under /etc/fstab"
		exit 1
	fi
  
elif [ "$FORMAT" == "ext2" ]; then

	export MMCSDP_AUTO_PARTITION=1
	echo "INFO: Formatting device type ext2"

elif [ "$FORMAT" == "ext3" ]; then

	export MMCSDP_AUTO_PARTITION=0
	echo "INFO: Device type ext3"
	cat /etc/fstab | grep ext3
	if [ $? -eq 1 ]; then
		echo "FATAL: Please add ext3 entry under /etc/fstab"
		exit 1
	fi

else

	echo "FATAL: Please specify the format of your device"
	echo "FATAL: Available values: dos, vfat, ext2, ext3"
	echo "e.g. export FORMAT=dos"
	exit 1

fi

# Remove any error file
handlerError.sh "clean"

# End of file

