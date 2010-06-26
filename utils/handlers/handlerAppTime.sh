#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_OPERATION=$1
LOCAL_INSTANCE=$2
LOCAL_IDENTIFIER=$3

# =============================================================================
# Functions
# =============================================================================

# None

# =============================================================================
# Main
# =============================================================================

handlerError.sh "test"
if [ "$?" -eq 1 ]; then
	exit 1
fi

if [ "$LOCAL_OPERATION" = "start" ]; then

	LOCAL_COMMAND=$4

	test -f $HATI_TIME_FINAL && rm $HATI_TIME_FINAL

	$UTILS_DIR_BIN/time -p -o $HATI_TIME.$LOCAL_INSTANCE.$LOCAL_IDENTIFIER $LOCAL_COMMAND

elif [ "$LOCAL_OPERATION" = "parse" ]; then

	cat $HATI_TIME.$LOCAL_INSTANCE.$LOCAL_IDENTIFIER | grep real | sed -e 's/[ a-z]//g' > $HATI_TIME_FINAL.$LOCAL_INSTANCE.$LOCAL_IDENTIFIER

fi

# End of file
