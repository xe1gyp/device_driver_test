#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_DD_INSTANCE=$1
LOCAL_DD_IF=$2
LOCAL_DD_OF=$3
LOCAL_DD_SIZE_CALC=$4
LOCAL_DD_SIZE=$5
LOCAL_DD_SYNC=$6

# =============================================================================
# Functions
# =============================================================================

logIt() {
	echo -e "$1"
	echo -e "$1" >> $HCDD_DD_LOG.$LOCAL_DD_INSTANCE
}

# =============================================================================
# Main
# =============================================================================

echo > $HCDD_DD_LOG.$LOCAL_DD_INSTANCE

handlerAppTop2.sh "start" $LOCAL_DD_INSTANCE

handlerAppTime.sh "start" $LOCAL_DD_INSTANCE "dd" "dd if=${LOCAL_DD_IF} of=${LOCAL_DD_OF} bs=${HCDD_DD_BS} count=${LOCAL_DD_SIZE}"

if [ "$LOCAL_DD_SYNC" != "nosync" ]; then
	handlerAppTime.sh "start" $LOCAL_DD_INSTANCE "sync" "sync"
fi

handlerAppTop2.sh "stop"  $LOCAL_DD_INSTANCE
handlerAppTop2.sh "parse" $LOCAL_DD_INSTANCE "User"
handlerAppTop2.sh "parse" $LOCAL_DD_INSTANCE "System"
handlerAppTime.sh "parse" $LOCAL_DD_INSTANCE "dd"

if [ "$LOCAL_DD_SYNC" != "nosync" ]; then
	handlerAppTime.sh "parse" $LOCAL_DD_INSTANCE "sync"
fi

echo 3 > /proc/sys/vm/drop_caches

TEMP_EXEC_TIME_DD=`cat $HATI_TIME_FINAL.$LOCAL_DD_INSTANCE.dd`

if [ "$LOCAL_DD_SYNC" != "nosync" ]; then
	TEMP_EXEC_TIME_SYNC=`cat $HATI_TIME_FINAL.$LOCAL_DD_INSTANCE.sync`
fi

if [ "$LOCAL_DD_SIZE_CALC" = "to" ]; then
	LOCAL_FILE_PATH=$LOCAL_DD_OF
elif [ "$LOCAL_DD_SIZE_CALC" = "from" ]; then
	LOCAL_FILE_PATH=$LOCAL_DD_IF
elif [ "$LOCAL_DD_SIZE_CALC" = "none" ]; then
	LOCAL_FILE_PATH=$LOCAL_DD_SIZE
fi

$UTILS_DIR_HANDLERS/handlerFileThroughput.sh none $LOCAL_DD_INSTANCE $LOCAL_FILE_PATH $TEMP_EXEC_TIME_DD $TEMP_EXEC_TIME_SYNC

THROUGHPUT_SIZE=`cat $HFT_THROUGHPUT_REAL_FILE_SIZE.$LOCAL_DD_INSTANCE`
THROUGHPUT_MBps=`cat $HFT_THROUGHPUT_MBps.$LOCAL_DD_INSTANCE`
THROUGHPUT_Mbps=`cat $HFT_THROUGHPUT_Mbps.$LOCAL_DD_INSTANCE`

logIt "Info: dd if=${LOCAL_DD_IF} of=${LOCAL_DD_OF} bs=${HCDD_DD_BS} count=${LOCAL_DD_SIZE}\n"
logIt "Info: Real File Size (MB) : $THROUGHPUT_SIZE"
logIt "Info: Exec Time dd   (s)  : $TEMP_EXEC_TIME_DD"

if [ "$LOCAL_DD_SYNC" != "nosync" ]; then
	logIt "Info: Exec Time sync (s)  : $TEMP_EXEC_TIME_SYNC"
fi

logIt "Info: Throughput (MB/s)   : $THROUGHPUT_MBps"
logIt "Info: Throughput (Mbps)   : $THROUGHPUT_Mbps"

cat $HCDD_DD_LOG.$LOCAL_DD_INSTANCE >> $HS_STATISTICS_GENERAL_FILE
cat $HAT_TOP2_DATA_LOG.$LOCAL_DD_INSTANCE >> $HS_STATISTICS_GENERAL_FILE
echo -e "\n---------------------------------------------------------------------\n" >>  $HS_STATISTICS_GENERAL_FILE 

# End of file

