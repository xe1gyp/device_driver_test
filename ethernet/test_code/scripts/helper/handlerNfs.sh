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

echo -e "\nThe content of /proc/cmdline is\n"
cat /proc/cmdline

echo -e "\nThe content of /proc/mounts for /dev/root is\n"
cat /proc/mounts | grep /dev/root

if [ "$LOCAL_OPERATION" = "verify" ]; then

	cat /proc/mounts | grep /dev/root | awk '{print $3}' | grep nfs
	if [ $? -eq 1 ]; then
		echo -e "\nFATAL: Root filesystem is not mounted over nfs\n"
		handlerError.sh "log" "$?" "halt" "handlerNfs.sh | verify operation"
	fi

	LOCAL_ETHERNET_NFS_DIRECTORY=`mount | grep nfs | awk '{print $3}'`
	echo $LOCAL_ETHERNET_NFS_DIRECTORY > $ETHERNET_NFS_DIR

elif [ "$LOCAL_OPERATION" = "deny" ]; then

	cat /proc/mounts | grep /dev/root | awk '{print $3}' | grep nfs

	if [ $? -eq 0 ]; then
		echo -e "\nFATAL: This test case should NOT be executed on a Network Filesystem\n"
		handlerError.sh "log" "1" "halt" "handlerNfs.sh | deny operation"
	fi

fi

# End of file
