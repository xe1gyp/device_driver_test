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

if [ "$LOCAL_OPERATION" = "clean" ]; then

	if [ -f $HE_ERROR_FILE_HALT ]; then
		rm -f $HE_ERROR_FILE_HALT
	fi
	if [ -f $HE_ERROR_FILE_CONTINUE ]; then
		rm -f $HE_ERROR_FILE_CONTINUE
	fi

elif [ "$LOCAL_OPERATION" = "log" ]; then

	LOCAL_ERROR=$2
	LOCAL_FLAG=$3
	LOCAL_HANDLER=$4

	if [ "$LOCAL_ERROR" -eq 1 ]; then
		if [ "$LOCAL_FLAG" = "halt" ]; then
			echo $LOCAL_HANDLER > $HE_ERROR_FILE_HALT
		elif [ "$LOCAL_FLAG" = "continue" ]; then
			echo $LOCAL_HANDLER > $HE_ERROR_FILE_CONTINUE
		fi
	fi

elif [ "$LOCAL_OPERATION" = "test" ];  then

	LOCAL_HANDLER=""

	if [ -f $HE_ERROR_FILE_HALT ]; then

		LOCAL_HANDLER=`cat $HE_ERROR_FILE_HALT`
		echo -e "\n\nFATAL: Error flag is set pointing to $LOCAL_HANDLER and will not continue\n\n"
		exit 1

	elif [ -f $HE_ERROR_FILE_CONTINUE ]; then

		LOCAL_HANDLER=`cat $HE_ERROR_FILE_CONTINUE`
		echo -e "\n\nWARNING: Error flag is set pointing to $LOCAL_HANDLER but will continue\n\n"
		exit 0

	fi

fi

# End of file
