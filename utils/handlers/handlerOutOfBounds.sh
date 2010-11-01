#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_COMMAND=$1

# =============================================================================
# Functions
# =============================================================================

# None

# =============================================================================
# Main
# =============================================================================

if [ $? -eq 1 ]; then
	exit 1
fi

LOCAL_SYSFS_ENTRY_NAME=$2
test -f $LOCAL_SYSFS_ENTRY_NAME
if [ $? != 0 ]; then
	handlerError.sh "log" "1" "halt" "handlerOutOfBounds.sh"
	exit 1
fi

if [ "$LOCAL_COMMAND" = "set" ]; then

	LOCAL_SYSFS_ENTRY_VALUE=$3
# The $? check MUST remain after the echo command or else the test will give a
# false negative
	echo $LOCAL_SYSFS_ENTRY_VALUE > $LOCAL_SYSFS_ENTRY_NAME
	if [ $? -eq 0 ]; then
		echo "Value of " $LOCAL_SYSFS_ENTRY_VALUE " was allowed"
		exit 1
	fi
	exit 0
fi

# End of file
