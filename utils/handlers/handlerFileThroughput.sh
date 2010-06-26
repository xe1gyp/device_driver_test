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
if [ "$?" -eq 1 ]; then
	exit 1
fi

if [ "$LOCAL_OPERATION" = "testusb" ]; then
  
	LOCAL_FIELD=$2
	LOCAL_ITERATIONS=$3
	LOCAL_FILE_SIZE=$4
	LOCAL_TIME=$5
  
	#             <iterations> <buffersize> <8>
	# througput = -----------------------------
	#                  1000 * 1000 * sec

	throughput=`echo "scale=4; ${LOCAL_ITERATIONS}*${LOCAL_FILE_SIZE}*8/(1000*1000*${LOCAL_TIME})" | bc`
	echo "Throughput (Mbps): ${throughput}"
	echo "Throughput (MB/s): "
	echo $throughput > $THROUGHPUT_VALUE

	if [ "$LOCAL_FIELD" = "average" ]; then
		echo $throughput > $GENERAL_THROUGHPUT_Mbps_AVR
	elif [ "$LOCAL_FIELD" = "maximum" ]; then
		echo $throughput > $GENERAL_THROUGHPUT_Mbps_MIN
	elif [ "$LOCAL_FIELD" = "minimum" ]; then
		echo $throughput > $GENERAL_THROUGHPUT_Mbps_MAX
	else
		exec
	fi

else
  
	LOCAL_INSTANCE=$2
	LOCAL_FILE_PATH=$3
	LOCAL_TIME_REAL_CMD=$4
	LOCAL_TIME_REAL_SYNC=$5
  
	#                             LOCAL_SIZE
	# throughput = ------------------------------------------
	#              LOCAL_TIME_REAL_CMD + LOCAL_TIME_REAL_SYNC
  
	if [ -e $LOCAL_FILE_PATH ]; then
		LOCAL_FILE_SIZE=`ls -al $LOCAL_FILE_PATH | awk '{print $5}'`
		LOCAL_FILE_SIZE=`echo "($LOCAL_FILE_SIZE/1024)/1024" | bc`
		echo $LOCAL_FILE_SIZE > $HFT_THROUGHPUT_REAL_FILE_SIZE.$LOCAL_INSTANCE
	else
		LOCAL_FILE_SIZE=$LOCAL_FILE_PATH
		echo $LOCAL_FILE_SIZE > $HFT_THROUGHPUT_REAL_FILE_SIZE.$LOCAL_INSTANCE
	fi

	if [ ! -z "$LOCAL_TIME_REAL_SYNC" ]; then
		LOCAL_TIME_REAL_TOTAL=`echo "scale=4; $LOCAL_TIME_REAL_CMD+$LOCAL_TIME_REAL_SYNC" | bc`
	else
		LOCAL_TIME_REAL_TOTAL=$LOCAL_TIME_REAL_CMD
	fi

	THROUGHPUT_MBps=`echo "scale=4; ${LOCAL_FILE_SIZE}/($LOCAL_TIME_REAL_TOTAL)" | bc`
	THROUGHPUT_Mbps=`echo "scale=4; ${THROUGHPUT_MBps}*8" | bc`

	echo $THROUGHPUT_MBps > $HFT_THROUGHPUT_MBps.$LOCAL_INSTANCE
	echo $THROUGHPUT_Mbps > $HFT_THROUGHPUT_Mbps.$LOCAL_INSTANCE

fi

# End of file
