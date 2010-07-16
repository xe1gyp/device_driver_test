#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_OPERATION=$1

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

if [ "$LOCAL_OPERATION" = "verify" ]; then

	cat /proc/mounts | grep /dev/root | awk '{print $3}' | grep nfs
	handlerError.sh "log" "$?" "halt" "handlerNfs.sh"

	LOCAL_ETHERNET_NFS_DIRECTORY=`mount | grep nfs | awk '{print $3}'`
	echo $LOCAL_ETHERNET_NFS_DIRECTORY > $ETHERNET_NFS_DIR

elif [ "$LOCAL_OPERATION" = "deny" ]; then

	cat /proc/mounts | grep /dev/root | awk '{print $3}' | grep nfs

	if [ $? -eq 0 ]; then
		echo "FATAL: This test case should NOT be executed on a Network Filesystem"
		handlerError.sh "log" "1" "halt" "handlerNfs.sh"
	fi

fi

# End of file
