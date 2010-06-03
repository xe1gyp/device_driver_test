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

if [ "$LOCAL_OPERATION" = "verify" ]; then

	cat /proc/mounts | grep /dev/root | awk '{print $3}' | grep nfs
	handlerError.sh "log" "$?" "halt" "handlerNfs.sh"

	LOCAL_ETHERNET_NFS_DIRECTORY=`mount | grep nfs | awk '{print $3}'`
	echo $LOCAL_ETHERNET_NFS_DIRECTORY > $ETHERNET_NFS_DIR

fi

# End of file
