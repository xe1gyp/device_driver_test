#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_OPERATION=$1
FS_RULES_FILE=$UTILS_DIR_TMP/fs_rules

ANDROID_RULES='-x /init;-d /dev/block;-d /dev/mtd;-d /system/bin'
UBUNTU_RULES='-x /sbin/init;-d /home'

# =============================================================================
# Functions
# =============================================================================

usage() {
	echo "Usage: handlerFilesystem.sh <command>"
	echo "Commands:"
	echo "	-getType: returns the Filesystem type (Android, Ubuntu, etc)"
}

analyzeFsStructure() {
	FS_RULES=${1:?Fatal Missing argument in analyzeFsStructure function}
	echo $FS_RULES | tr ";" "\n" > $FS_RULES_FILE

	FLAG=0

	while read line
	do
		if [ ! $line ]; then
			FLAG=1
		fi
	done < "$FS_RULES_FILE"

	return $FLAG
}

getType() {
	analyzeFsStructure "$ANDROID_RULES"
	if [ $? = 0 ]; then
		echo android > "$HFS_FS_TYPE"
		return 0
	fi

	analyzeFsStructure "$UBUNTU_RULES"
	if [ $? = 0 ]; then
		echo ubuntu > $HFS_FS_TYPE
		return 0
	fi

	echo unknown > $HFS_FS_TYPE
	return 1
}

# =============================================================================
# Main
# =============================================================================

if [ "$LOCAL_OPERATION" = "getType" ]; then
	getType
elif [ -z "$LOCAL_OPERATION" ]; then
	usage
	exit 1
else
	echo "Command $LOCAL_OPERATION is not supported"
	exit 1
fi
