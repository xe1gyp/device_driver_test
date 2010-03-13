#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_INSTANCE=$1
LOCAL_SOURCE=$2
LOCAL_DESTINATION=$3

# =============================================================================
# Functions
# =============================================================================

logIt() {
  echo -e "$1"
  echo -e "$1" >> $HCCP_LOG_FILE.$LOCAL_INSTANCE
}

# =============================================================================
# Main
# =============================================================================

echo > $HCCP_LOG_FILE.$LOCAL_INSTANCE

$UTILS_DIR_HANDLERS/handlerAppTop2.sh $LOCAL_INSTANCE "start"
$UTILS_DIR_BIN/time -p -o $HCCP_EXECUTION_TIME_CP.$LOCAL_INSTANCE cp $LOCAL_SOURCE $LOCAL_DESTINATION
$UTILS_DIR_BIN/time -p -o $HCCP_EXECUTION_TIME_SYNC.$LOCAL_INSTANCE sync
$UTILS_DIR_HANDLERS/handlerAppTop2.sh $LOCAL_INSTANCE "stop"
$UTILS_DIR_HANDLERS/handlerAppTop2.sh $LOCAL_INSTANCE "parse" "User"
$UTILS_DIR_HANDLERS/handlerAppTop2.sh $LOCAL_INSTANCE "parse" "System"

echo 3 > /proc/sys/vm/drop_caches

#cat $HAT_CPU_USAGE_VALUE_AVR.$LOCAL_INSTANCE
#cat $HAT_CPU_USAGE_VALUE_MAX.$LOCAL_INSTANCE
#cat $HAT_CPU_USAGE_VALUE_MIN.$LOCAL_INSTANCE

LOCAL_FILE_SIZE=`ls -al ${LOCAL_DESTINATION} | awk '{print $5}'`
LOCAL_FILE_SIZE=`echo "(${LOCAL_FILE_SIZE}/1024)/1024" | bc`
TEMP_EXEC_TIME_CP=`cat $HCCP_EXECUTION_TIME_CP.$LOCAL_INSTANCE | grep real | sed -e 's/[ a-z]//g'`
TEMP_EXEC_TIME_SYNC=`cat $HCCP_EXECUTION_TIME_SYNC.$LOCAL_INSTANCE | grep real | sed -e 's/[ a-z]//g'`
$UTILS_DIR_HANDLERS/handlerFileThroughput.sh none $LOCAL_INSTANCE $LOCAL_FILE_SIZE $TEMP_EXEC_TIME_CP $TEMP_EXEC_TIME_SYNC
THROUGHPUT_MBps=`cat $HFT_THROUGHPUT_MBps.$LOCAL_INSTANCE`
THROUGHPUT_Mbps=`cat $HFT_THROUGHPUT_Mbps.$LOCAL_INSTANCE`

logIt "\nInfo: cp $LOCAL_SOURCE $LOCAL_DESTINATION\n"
logIt "Info: Real File Size (MB) : $LOCAL_FILE_SIZE"
logIt "Info: Exec Time cp   (s)  : $TEMP_EXEC_TIME_CP"
logIt "Info: Exec Time sync (s)  : $TEMP_EXEC_TIME_SYNC"
logIt "Info: Throughput (MB/s)   : $THROUGHPUT_MBps"
logIt "Info: Throughput (Mbps)   : $THROUGHPUT_Mbps"

cat $HCCP_LOG_FILE.$LOCAL_INSTANCE >> $HS_STATISTICS_GENERAL_FILE

# End of file