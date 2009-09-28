#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_OPERATION=$1

# =============================================================================
# Main
# =============================================================================

if [ "$LOCAL_OPERATION" = "create" ]; then

  LOCAL_TMPFS_SIZE=$2
  LOCAL_TMPFS_MOUNTPOINT=$3

  test -d $LOCAL_TMPFS_MOUNTPOINT || mkdir -p $LOCAL_TMPFS_MOUNTPOINT

	mount -t tmpfs tmpfs $LOCAL_TMPFS_MOUNTPOINT -o size=${LOCAL_TMPFS_SIZE}M
  mount

elif [ "$LOCAL_OPERATION" = "remove" ]; then

  LOCAL_TMPFS_MOUNTPOINT=$2

  sync && umount $LOCAL_TMPFS_MOUNTPOINT
  rm -rf $LOCAL_TMPFS_MOUNTPOINT
  mount

fi

# End of file
