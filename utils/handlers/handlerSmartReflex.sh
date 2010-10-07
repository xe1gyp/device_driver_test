#!/bin/bash

#
# TODO
# 1. Include "all" parameter to set all domains at once
#

# =============================================================================
# Variables
# =============================================================================

LOCAL_OPERATION=$1
LOCAL_DOMAIN=$2
LOCAL_ERROR=0

# =============================================================================
# Functions
# =============================================================================

# None

# =============================================================================
# Main
# =============================================================================

handlerError.sh "test"
if [ $? -eq 1 ]; then
	exit 1
fi

# Define which Architecture is being used
if [ `cat /proc/cpuinfo | grep -ic OMAP4` -gt 0 ]; then
		LOCAL_SR_ENTRIES=($SR_CORE_AUTOCOMP $SR_IVA_AUTOCOMP $SR_MPU_AUTOCOMP)
		LOCAL_SR_DOMAIN=(core iva mpu)
		echo "OMAP4 Architecture detected"
	elif [ `cat /proc/cpuinfo | grep -ic Zoom3` -gt 0 ]; then
		LOCAL_SR_ENTRIES=($SR_VDD1_AUTOCOMP $SR_VDD2_AUTOCOMP)
		LOCAL_SR_DOMAIN=(vdd1 vdd2)
		echo "OMAP3 Architecture detected"
	else
		echo "FATAL: Architecture not detected" 1>&2
		exit 1
fi


# Define Script Usage and validate all parameters
if [ $# -ne 2 ]; then
		echo "ERR: number of parameters is invalid" 1>&2
		echo "Try -- $0 <enable/disable> <${LOCAL_SR_DOMAIN[*]}>" 1>&2
		exit 1
fi

if [ $LOCAL_OPERATION = "enable" ]; then
		LOCAL_STATUS=$PM_ENABLE
	elif [ $LOCAL_OPERATION = "disable" ]; then
		LOCAL_STATUS=$PM_DISABLE
	else
		echo "ERR: "$LOCAL_OPERATION" is an invalid parameter" 1>&2
		echo "Try -- $0 <enable/disable> <${LOCAL_SR_DOMAIN[*]}>" 1>&2
		exit 1
fi

if [ `echo ${LOCAL_SR_DOMAIN[*]} | grep -ic $LOCAL_DOMAIN` -eq 0  ]; then
	echo "ERR: "$LOCAL_DOMAIN" is an invalid parameter" 1>&2
	echo "ERR: valid domain parameters are <${LOCAL_SR_DOMAIN[*]}>" 1>&2
	echo "Try -- $0 <enable/disable> <${LOCAL_SR_DOMAIN[*]}>" 1>&2
	exit 1
fi

# Verify that all sysfs entries for SmartReflex exists

handlerDebugFileSystem.sh "mount"

for sr_entry in ${LOCAL_SR_ENTRIES[*]}; do
	if [ ! -f $sr_entry ]; then
		echo "FATAL: $sr_entry cannot be found" 1>&2
		LOCAL_ERROR=1
	fi
done

if [ $LOCAL_ERROR -eq 1 ]; then
	handlerError.sh "log" "1" "halt" "handlerSmartReflex.sh"
	handlerDebugFileSystem.sh "umount"
	exit $LOCAL_ERROR
fi

# Set SmartReflex autocompensation

for index in ${!LOCAL_SR_DOMAIN[*]}; do
	if [ "$LOCAL_DOMAIN" = ${LOCAL_SR_DOMAIN[$index]} ]; then
		echo "echo $LOCAL_STATUS > ${LOCAL_SR_ENTRIES[$index]}"
		handlerSysFs.sh "set"  ${LOCAL_SR_ENTRIES[$index]} $LOCAL_STATUS
		handlerSysFs.sh "compare"  ${LOCAL_SR_ENTRIES[$index]} $LOCAL_STATUS
		if [ $? -ne 0 ]; then
			echo "ERR: ${LOCAL_SR_ENTRIES[$index]} was not set properly" 1>&2
			LOCAL_ERROR=1
		fi
	fi
done

if [ $LOCAL_ERROR -eq 1 ]; then
        handlerError.sh "log" "1" "halt" "handlerSmartReflex.sh"
        handlerDebugFileSystem.sh "umount"
        exit $LOCAL_ERROR
fi

handlerDebugFileSystem.sh "umount"
exit $LOCAL_ERROR

# End of file
