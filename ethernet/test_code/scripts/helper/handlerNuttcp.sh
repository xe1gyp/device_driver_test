#!/bin/sh

set -x

LOCAL_COMMAND=$1
LOCAL_ARGUMENTS=$2

LOCAL_THROUGHPUT_FILE=$ETHERNET_DIR_TMP/handlerNuttcp.local.throughput
LOCAL_DROPPED_FILE=$ETHERNET_DIR_TMP/handlerNuttcp.local.dropped

if [ -z $NUTTCP_SERVER ]; then
	echo "FATAL: Cannot find NUTTCP_SERVER variable exported, please check!"
	exit 1
fi

echo $NUTTCP_SERVER > $ETHERNET_NUTTCP_SERVER_IPADDR

if [ "$LOCAL_COMMAND" = "throughput" ]; then

	LOCAL_DATA=$3
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

elif [ "$LOCAL_COMMAND" = "dropped" ]; then

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
