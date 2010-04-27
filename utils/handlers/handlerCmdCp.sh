#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_CP_INSTANCE=$1
LOCAL_CP_SOURCE=$2
LOCAL_CP_DESTINATION=$3
LOCAL_CP_SYNC=$4

# =============================================================================
# Functions
# =============================================================================

logIt() {
	echo -e "$1"
	echo -e "$1" >> $HCCP_CP_LOG.$LOCAL_CP_INSTANCE
}

# =============================================================================
# Main
# =============================================================================

echo > $HCCP_CP_LOG.$LOCAL_CP_INSTANCE

handlerAppTop2.sh "start" $LOCAL_CP_INSTANCE

handlerAppTime.sh "start" $LOCAL_CP_INSTANCE "cp" "cp $LOCAL_CP_SOURCE $LOCAL_CP_DESTINATION"

if [ "$LOCAL_CP_SYNC" != "nosync" ]; then
	handlerAppTime.sh "start" $LOCAL_CP_INSTANCE "sync" "sync"
fi

handlerAppTop2.sh "stop"  $LOCAL_CP_INSTANCE
handlerAppTop2.sh "parse" $LOCAL_CP_INSTANCE "User"
handlerAppTop2.sh "parse" $LOCAL_CP_INSTANCE "System"
handlerAppTime.sh "parse" $LOCAL_CP_INSTANCE "cp"

if [ "$LOCAL_CP_SYNC" != "nosync" ]; then
	handlerAppTime.sh "parse" $LOCAL_CP_INSTANCE "sync"
fi

echo 3 > /proc/sys/vm/drop_caches

TEMP_EXEC_TIME_CP=`cat $HATI_TIME_FINAL.$LOCAL_CP_INSTANCE.cp`

if [ "$LOCAL_CP_SYNC" != "nosync" ]; then
	TEMP_EXEC_TIME_SYNC=`cat $HATI_TIME_FINAL.$LOCAL_CP_INSTANCE.sync`
fi

$UTILS_DIR_HANDLERS/handlerFileThroughput.sh none $LOCAL_CP_INSTANCE $LOCAL_CP_DESTINATION $TEMP_EXEC_TIME_CP $TEMP_EXEC_TIME_SYNC

THROUGHPUT_SIZE=`cat $HFT_THROUGHPUT_REAL_FILE_SIZE.$LOCAL_CP_INSTANCE`
THROUGHPUT_MBps=`cat $HFT_THROUGHPUT_MBps.$LOCAL_CP_INSTANCE`
THROUGHPUT_Mbps=`cat $HFT_THROUGHPUT_Mbps.$LOCAL_CP_INSTANCE`

logIt "\nInfo: cp $LOCAL_CP_SOURCE $LOCAL_CP_DESTINATION\n"
logIt "Info: Real File Size (MB) : $THROUGHPUT_SIZE"
logIt "Info: Exec Time cp   (s)  : $TEMP_EXEC_TIME_CP"

if [ "$LOCAL_CP_SYNC" != "nosync" ]; then
	logIt "Info: Exec Time sync (s)  : $TEMP_EXEC_TIME_SYNC"
fi

logIt "Info: Throughput (MB/s)   : $THROUGHPUT_MBps"
logIt "Info: Throughput (Mbps)   : $THROUGHPUT_Mbps"

cat $HCCP_CP_LOG.$LOCAL_CP_INSTANCE >> $HS_STATISTICS_GENERAL_FILE

# End of file

