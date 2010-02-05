#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_DEVICE=$1

# =============================================================================
# Functions
# =============================================================================

# =============================================================================
# Main
# =============================================================================

if [ "$LOCAL_DEVICE" = "nand" ]; then

  MTD_FILE=`cat /proc/mtd | grep "File System - NAND" | cut -d ':' -f0`
  export LOCAL_CHARACTER_DEVICE=/dev/$MTD_FILE
  MTD_NUMBER=`echo $MTD_FILE | cut -c4-`
  export LOCAL_BLOCK_DEVICE=/dev/mtdblock${MTD_NUMBER}
  LOCAL_JFFS_OPTIONS="-j"

elif [ "$LOCAL_DEVICE" = "onenand" ]; then

  MTD_FILE=`cat /proc/mtd | grep OneNAND | grep "File" | cut -d ':' -f0`
  export LOCAL_CHARACTER_DEVICE=/dev/$MTD_FILE
  MTD_NUMBER=`echo $MTD_FILE | cut -c4-`
  export LOCAL_BLOCK_DEVICE=/dev/mtdblock${MTD_NUMBER}
  LOCAL_JFFS_OPTIONS=""

elif [ "$LOCAL_DEVICE" = "nor" ]; then

  MTD_FILE=`cat /proc/mtd | grep NOR | grep "File" | cut -d ':' -f0`
  export LOCAL_CHARACTER_DEVICE=/dev/$MTD_FILE
  MTD_NUMBER=`echo $MTD_FILE | cut -c4-`
  export MAIN_PARTITION=$MTD_NUMBER
  export LOCAL_BLOCK_DEVICE=/dev/mtdblock${MTD_NUMBER}
  LOCAL_JFFS_OPTIONS=""

else

  echo "The memory type has not being specified is not valid" && exit 1

fi

$MTD_DIR_BINARIES/flash_eraseall $LOCAL_JFFS_OPTIONS $LOCAL_CHARACTER_DEVICE

echo $LOCAL_JFFS_OPTIONS > $MTD_JFFS_OPTIONS.$LOCAL_DEVICE
echo $LOCAL_CHARACTER_DEVICE > $MTD_CHARACTER_DEVICE.$LOCAL_DEVICE
echo $LOCAL_BLOCK_DEVICE > $MTD_BLOCK_DEVICE.$LOCAL_DEVICE

# End of file