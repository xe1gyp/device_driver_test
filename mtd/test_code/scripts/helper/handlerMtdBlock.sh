#!/bin/sh

set -x

# =============================================================================
# Local Variables
# =============================================================================

LOCAL_COMMAND=$1

# =============================================================================
# Functions
# =============================================================================

fstabModifier() {
  if [ "$LOCAL_COMMAND" = "create" ]; then
    echo "${LOCAL_BLOCK_DEVICE} ${LOCAL_MOUNT_POINT} jffs2 defaults 0 0" >> /etc/fstab
    cat /etc/fstab
  fi
}

# =============================================================================
# Main
# =============================================================================

if [ "$LOCAL_COMMAND" = "create" ]; then

  LOCAL_DEVICE=$2
  LOCAL_MOUNT_POINT=$3

  $MTD_DIR_HELPER/handlerMtdSetup.sh $LOCAL_DEVICE

  LOCAL_BLOCK_DEVICE=`cat $MTD_BLOCK_DEVICE.$LOCAL_DEVICE`

  test -d $LOCAL_MOUNT_POINT || mkdir -p $LOCAL_MOUNT_POINT

	mount -t jffs2 $LOCAL_BLOCK_DEVICE $LOCAL_MOUNT_POINT
	mount

elif [ "$LOCAL_COMMAND" = "remove" ]; then

  LOCAL_MOUNT_POINT=$2

  sync && umount $LOCAL_MOUNT_POINT && sync
  sleep 5
  mount

  test -d $LOCAL_MOUNT_POINT && rm -rf $LOCAL_MOUNT_POINT

fi

# End of file
