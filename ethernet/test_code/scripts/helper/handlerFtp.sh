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

handlerError.sh "test"
if [ $? -eq 1 ]; then
	exit 1
fi

if [ "$LOCAL_OPERATION" = "check" ]; then

	LOCAL_ERROR=0

	if [ -z $FTP_SERVER ]; then
		echo "FATAL: Cannot find FTP_SERVER variable exported, please check!"
		LOCAL_ERROR=1
	fi

	if [ -z $FTP_USER ]; then
		echo "FATAL: Cannot find FTP_USER variable exported, please check!"
		LOCAL_ERROR=1
	fi

	if [ -z $FTP_PASSWD ]; then
		echo "FATAL: Cannot find FTP_PASSWD variable exported, please check!"
		LOCAL_ERROR=1
	fi

	if [ $LOCAL_ERROR -eq 1 ]; then
		echo "FATAL: One or more variables needed for FTP testing have not been defined!"
		handlerError.sh "log" "1" "halt" "handlerFtp.sh"
		exit 2
	fi

elif [ "$LOCAL_OPERATION" = "put" ]; then

	LOCAL_ARGUMENTS=$2

	ftpput $LOCAL_ARGUMENTS

elif [ "$LOCAL_OPERATION" = "get" ]; then

	LOCAL_ARGUMENTS=$2

	ftpget $LOCAL_ARGUMENTS

fi

# End of file
