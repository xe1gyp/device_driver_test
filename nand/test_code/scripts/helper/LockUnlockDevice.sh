#!/bin/sh

DEVICE=$1
OPERATION=$2
OFFSET=$3
ERASE_BLOCKS=$4

if [ "$OPERATION" == "Lock"  ]; then
   $TESTBIN/flash_lock $DEVICE $OFFSET $ERASE_BLOCKS
elif [ "$OPERATION" == "Unlock" ]; then
   $TESTBIN/flash_unlock $DEVICE $OFFSET $ERASE_BLOCKS
else
   echo "Operation not recognized!"
   exit 1
fi
