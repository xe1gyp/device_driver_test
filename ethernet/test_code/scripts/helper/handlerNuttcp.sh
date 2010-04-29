#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_MODE=$1
LOCAL_OPERATION=$2
LOCAL_ARGUMENTS=$3

LOCAL_THROUGHPUT_FILE=$ETHERNET_DIR_TMP/handlerNuttcp.local.throughput
LOCAL_DROPPED_FILE=$ETHERNET_DIR_TMP/handlerNuttcp.local.dropped

# =============================================================================
# Functions
# =============================================================================

# None

# =============================================================================
# Main
# =============================================================================

handlerError.sh "test"
if [ $? -eq 1 ]; then
	return 1
fi

if [ "$LOCAL_MODE" = "server" ]; then

	if [ "$LOCAL_OPERATION" = "start" ]; then
		nuttcp -S
	elif [ "$LOCAL_OPERATION" = "stop" ]; then
		killall nuttcp
	fi

	exit 0

elif [ "$LOCAL_MODE" = "remote" ]; then

	if [ -z $NUTTCP_SERVER ]; then
		echo "FATAL: Cannot find NUTTCP_SERVER variable exported, please check!"
		exit 2
	fi

elif [ "$LOCAL_MODE" = "local" ]; then

	NUTTCP_SERVER=127.0.0.1

fi

echo $NUTTCP_SERVER > $ETHERNET_NUTTCP_SERVER_IPADDR

if [ "$LOCAL_OPERATION" = "common" ]; then

        eval nuttcp "$LOCAL_ARGUMENTS" `cat $ETHERNET_NUTTCP_SERVER_IPADDR`

elif [ "$LOCAL_OPERATION" = "throughput" ]; then

	LOCAL_DATA=$4
	nuttcp "$LOCAL_ARGUMENTS" `cat $ETHERNET_NUTTCP_SERVER_IPADDR` | awk '{print $7}' > $LOCAL_THROUGHPUT_FILE
	LOCAL_THROUGHPUT=`cat $LOCAL_THROUGHPUT_FILE`
	echo "Throughput Minimun Required: $LOCAL_DATA"
	echo "Throughput Final: $LOCAL_THROUGHPUT"
	LOCAL_RESULT=`echo "$LOCAL_THROUGHPUT > $LOCAL_DATA" | bc`

	if [ "$LOCAL_RESULT" == "1" ]; then
		echo "Good Throughput!"
		exit 0
	else
		echo "Bad Throughput!"
		exit 1
	fi

elif [ "$LOCAL_OPERATION" = "dropped" ]; then

	nuttcp "$LOCAL_ARGUMENTS" `cat $ETHERNET_NUTTCP_SERVER_IPADDR` | awk '{print $17}' > $LOCAL_DROPPED_FILE
	LOCAL_DROPPED=`cat $LOCAL_DROPPED_FILE`
	echo "Dropped Packets Final: $LOCAL_DROPPED"
	LOCAL_RESULT=`echo "$LOCAL_DROPPED > 0" | bc`

	if [ "$LOCAL_RESULT" == "1" ]; then
		echo "Some Packets Dropped!"
		exit 1
	else
		echo "No Packets Dropped!"
		exit 0
	fi

fi

# End of file
