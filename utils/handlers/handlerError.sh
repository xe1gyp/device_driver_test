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

	test -f $HE_ERROR_FILE_HALT && rm -f $HE_ERROR_FILE_HALT
	test -f $HE_ERROR_FILE_CONTINUE && rm -f $HE_ERROR_FILE_CONTINUE

elif [ "$LOCAL_OPERATION" = "log" ]; then

	LOCAL_ERROR=$2
	LOCAL_FLAG=$3
	LOCAL_HANDLER=$4

	if [ "$LOCAL_ERROR" -eq 1 ]; then
		if [ "$LOCAL_FLAG" == "halt" ]; then
			echo $LOCAL_HANDLER > $HE_ERROR_FILE_HALT
		elif [ "$LOCAL_FLAG" == "continue" ]; then
			echo $LOCAL_HANDLER > $HE_ERROR_FILE_CONTINUE
		fi
	fi

elif [ "$LOCAL_OPERATION" = "test" ];  then

	LOCAL_HANDLER=""

	if [ -f $HE_ERROR_FILE_HALT ]; then

		LOCAL_HANDLER=`cat $HE_ERROR_FILE_HALT`
		echo -e "\nFATAL: Error flag is set pointing to $LOCAL_HANDLER and will not continue\n"
		return 1

	elif [ -f $HE_ERROR_FILE_CONTINUE ]; then

		LOCAL_HANDLER=`cat $HE_ERROR_FILE_CONTINUE`
		echo -e "\nWARNING: Error flag is set pointing to $LOCAL_HANDLER but will continue\n"
		return 0

	fi

fi

# End of file
