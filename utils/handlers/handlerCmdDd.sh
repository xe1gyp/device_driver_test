#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_INSTANCE=$1
LOCAL_OF=$2
LOCAL_SIZE=$3

LOCAL_BS="1M"
LOCAL_IF=/dev/urandom

# =============================================================================
# Functions
# =============================================================================

logIt() {
  echo -e "$1"
  echo -e "$1" >> $HCDD_DD_LOG.$LOCAL_INSTANCE
}

# =============================================================================
# Main
# =============================================================================

echo > $HCDD_DD_LOG.$LOCAL_INSTANCE

$UTILS_DIR_HANDLERS/handlerAppTop2.sh $LOCAL_INSTANCE "start"
$UTILS_DIR_BIN/time -p -o $HCDD_EXECUTION_TIME_DD.$LOCAL_INSTANCE dd if=${LOCAL_IF} of=${LOCAL_OF} bs=${LOCAL_BS} count=${LOCAL_SIZE}
$UTILS_DIR_BIN/time -p -o $HCDD_EXECUTION_TIME_SYNC.$LOCAL_INSTANCE sync
$UTILS_DIR_HANDLERS/handlerAppTop2.sh $LOCAL_INSTANCE "stop"
$UTILS_DIR_HANDLERS/handlerAppTop2.sh $LOCAL_INSTANCE "parse" "User"
$UTILS_DIR_HANDLERS/handlerAppTop2.sh $LOCAL_INSTANCE "parse" "System"

echo 3 > /proc/sys/vm/drop_caches

#cat $HAT_CPU_USAGE_VALUE_AVR.$LOCAL_INSTANCE
#cat $HAT_CPU_USAGE_VALUE_MAX.$LOCAL_INSTANCE
#cat $HAT_CPU_USAGE_VALUE_MIN.$LOCAL_INSTANCE

LOCAL_FILE_SIZE=`ls -al ${LOCAL_OF} | awk '{print $5}'`
LOCAL_FILE_SIZE=`echo "(${LOCAL_FILE_SIZE}/1024)/1024" | bc`

TEMP_HCDD_EXEC_TIME_DD=`cat $HCDD_EXECUTION_TIME_DD.$LOCAL_INSTANCE | grep real | sed -e 's/[ a-z]//g'`
TEMP_HCDD_EXEC_TIME_SYNC=`cat $HCDD_EXECUTION_TIME_SYNC.$LOCAL_INSTANCE | grep real | sed -e 's/[ a-z]//g'`
$UTILS_DIR_HANDLERS/handlerFileThroughput.sh none $LOCAL_INSTANCE $LOCAL_FILE_SIZE $TEMP_HCDD_EXEC_TIME_DD $TEMP_HCDD_EXEC_TIME_SYNC
THROUGHPUT_MBps=`cat $HFT_THROUGHPUT_MBps.$LOCAL_INSTANCE`
THROUGHPUT_Mbps=`cat $HFT_THROUGHPUT_Mbps.$LOCAL_INSTANCE`

logIt "\nInfo: dd if=${LOCAL_IF} of=${LOCAL_OF} bs=${LOCAL_BS} count=${LOCAL_SIZE}\n"
logIt "Info: Real File Size (MB) : $LOCAL_FILE_SIZE"
logIt "Info: Exec Time dd   (s)  : $TEMP_HCDD_EXEC_TIME_DD"
logIt "Info: Exec Time sync (s)  : $TEMP_HCDD_EXEC_TIME_SYNC"
logIt "Info: Throughput (MB/s)   : $THROUGHPUT_MBps"
logIt "Info: Throughput (Mbps)   : $THROUGHPUT_Mbps"

cat $HCDD_DD_LOG.$LOCAL_INSTANCE >> $HS_STATISTICS_GENERAL_FILE
cat $HAT_TOP2_DATA_LOG.$LOCAL_INSTANCE >> $HS_STATISTICS_GENERAL_FILE

echo -e "\n---------------------------------------------------------------------\n" >>  $HS_STATISTICS_GENERAL_FILE 

# End of file