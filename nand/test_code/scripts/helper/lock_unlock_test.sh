#!/bin/sh

OPERATION=$1
OFFSET=$2
ERASE_BLOCKS=$3

if [ "$OPERATION" == "Lock"  ]; then
   $TESTBIN/flash_lock $MTD_CHAR_DEV1 $OFFSET $ERASE_BLOCKS
elif [ "$OPERATION" == "Unlock" ]; then
   $TESTBIN/flash_unlock $MTD_CHAR_DEV1 $OFFSET $ERASE_BLOCKS
else
   echo "Operation not recognized!"
   exit 1
fi && $TESTBIN/flash_erase $MTD_CHAR_DEV1 $OFFSET $ERASE_BLOCKS
