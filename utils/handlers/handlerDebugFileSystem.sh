#!/bin/sh

LOCAL_OPERATION=$1

if [ "$LOCAL_COMMAND" = "mount" ]; then

	test -d $HDFS_DEBUG_FILESYSTEM_DIRECTORY || mkdir -p $HDFS_DEBUG_FILESYSTEM_DIRECTORY

	mount | grep debugfs

	if [ $? != 0 ]; then
		mount -t debugfs debugfs $HDFS_DEBUG_FILESYSTEM_DIRECTORY
	fi


elif [ "$LOCAL_COMMAND" = "umount" ]; then

	umount $HDFS_DEBUG_FILESYSTEM_DIRECTORY
	rmdir $HDFS_DEBUG_FILESYSTEM_DIRECTORY

fi

# End of file
