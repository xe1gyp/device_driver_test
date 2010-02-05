#!/bin/sh

LOCAL_COMMAND=$1
LOCAL_STRING_TO_SEARCH=$2

if [ "$LOCAL_COMMAND" = "erase" ]; then

	echo > $KERNEL_MESSAGES

elif [ "$LOCAL_COMMAND" = "search" ]; then

	cat $KERNEL_MESSAGES | grep $LOCAL_STRING_TO_SEARCH

	if [ $? != 0 ]; then
		echo "Error: String could not be found"
		cat $KERNEL_MESSAGES
		exit 1
	fi
  
else
	echo "Command in helperKernelMessages.sh is not supported"
	exit 1
fi

# End of file
