#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_COMMAND=$1

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

if [ "$LOCAL_COMMAND" = "mount" ]; then

	test -d $HDFS_DEBUG_FILESYSTEM_DIRECTORY || mkdir -p $HDFS_DEBUG_FILESYSTEM_DIRECTORY

	mount | grep debugfs | grep "debugfs on $HDFS_DEBUG_FILESYSTEM_DIRECTORY"

	if [ $? != 0 ]; then
		mount -t debugfs debugfs $HDFS_DEBUG_FILESYSTEM_DIRECTORY
	fi

elif [ "$LOCAL_COMMAND" = "set" ]; then

	LOCAL_DEBUGFS_ENTRY_VALUE=$2
	LOCAL_DEBUGFS_ENTRY_NAME=$3

	handlerDebugFileSystem.sh "mount"

	test -f $LOCAL_DEBUGFS_ENTRY_NAME
	if [ $? != 0 ]; then
		handlerError.sh "log" "1" "halt" "handlerSysFs.sh"
		exit 1
	fi

	echo $LOCAL_DEBUGFS_ENTRY_VALUE > $LOCAL_DEBUGFS_ENTRY_NAME

	handlerDebugFileSystem.sh "umount"

elif [ "$LOCAL_COMMAND" = "umount" ]; then

	umount $HDFS_DEBUG_FILESYSTEM_DIRECTORY

fi

# End of file
