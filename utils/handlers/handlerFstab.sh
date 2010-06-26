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

if [ "$LOCAL_OPERATION" = "save" ]; then

	cat $HFST_FSTAB_NAME > $HFST_FSTAB_ORIGINAL

elif [ "$LOCAL_OPERATION" = "add" ]; then

	LOCAL_PARTITION_NUMBER=$2
	LOCAL_FSTAB_LINE=$3

	echo $LOCAL_FSTAB_LINE > $HFST_FSTAB_ENTRY.$LOCAL_PARTITION_NUMBER
	echo $LOCAL_FSTAB_LINE >> $HFST_FSTAB_NAME

elif [ "$LOCAL_OPERATION" = "get" ]; then

	LOCAL_PARTITION_NUMBER=$2

	cat $HFST_FSTAB_ENTRY.$LOCAL_PARTITION_NUMBER >> $HFST_FSTAB_NAME

elif [ "$LOCAL_OPERATION" = "restore" ]; then

	cat $HFST_FSTAB_ORIGINAL > $HFST_FSTAB_NAME

fi

# End of file

