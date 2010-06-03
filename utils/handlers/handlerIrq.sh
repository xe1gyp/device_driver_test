#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_OPERATION=$1
LOCAL_DRIVER=$2
LOCAL_FILE=$3

# =============================================================================
# Functions
# =============================================================================

# None

# =============================================================================
# Main
# =============================================================================

if [ "$LOCAL_OPERATION" = "get" ]; then
	if [ "$LOCAL_DRIVER" = "" ]; then
		echo "Error: driver value was not provided"
		exit 1
	fi

	for i in `cat /proc/interrupts | grep -i $LOCAL_DRIVER | awk '{print$1}' | cut -d: -f1`
	do
		 echo $i > $LOCAL_FILE
	done
else
	echo "Fatal: Unsupported operation"
	exit 1
fi

# End of file
