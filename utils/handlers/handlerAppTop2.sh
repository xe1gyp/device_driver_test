#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_INSTANCE=$1
LOCAL_COMMAND=$2
Top2Scale=0

# =============================================================================
# Functions
# =============================================================================

logIt() {

  echo -e $1
  echo -e $1 >> $HAT_LOG_FILE.$LOCAL_INSTANCE

}

getAverage() {

  # Get average value from tmp_file
  acc=0
  temp=0
  n=0
  while read line
  do
    temp=`echo $line|bc`
	acc=`echo "scale=$Top2Scale; $temp+$acc" | bc`
	n=`echo "$n+1"|bc`
  done < $HAT_TOP2_DATA_TEMPORAL.$LOCAL_INSTANCE
  average=`echo "scale=$Top2Scale; $acc/$n"|bc`
  echo -e "Info: Average Value: ${average}"
  echo $average > $HAT_CPU_USAGE_VALUE_AVR.$LOCAL_INSTANCE


}

getMaximum() {

  # Get maximum value from tmp_file
  temp=0
  max=0
  while read line
  do
    temp=`echo $line|bc`
    max_ans=`echo "scale=$Top2Scale; $temp>$max" | bc`
    if [ $max_ans = 1 ]; then
      max=$temp
      if [ "$DEBUG" = "YES" ]; then
        echo "Current Value: ${max}"
      fi
    fi
  done < $HAT_TOP2_DATA_TEMPORAL.$LOCAL_INSTANCE
  echo -e "Info: Maximum Value: ${max}"
  echo $max > $HAT_CPU_USAGE_VALUE_MAX.$LOCAL_INSTANCE

}

getMinimum() {

  # Get minimum value from tmp_file
  temp=0
  min=$average
  while read line
  do
    temp=`echo $line|bc`
    min_ans=`echo "scale=$Top2Scale; $temp<$min" | bc`
    if [ $min_ans = 1 ]; then
      min=$temp
      if [ "$DEBUG" = "YES" ]; then
        echo "Current Value: ${min}"
      fi
    fi
  done < $HAT_TOP2_DATA_TEMPORAL.$LOCAL_INSTANCE
  echo -e "Info: Minimum Value: ${min}"
  echo $min > $HAT_CPU_USAGE_VALUE_MIN.$LOCAL_INSTANCE

}

# =============================================================================
# Main
# =============================================================================

if [ "$LOCAL_COMMAND" = "start" ]; then

  echo > $HAT_LOG_FILE.$LOCAL_INSTANCE

  # Start top2 data logging
  cp $UTILS_DIR_BIN/top2 $UTILS_DIR_BIN/top2.$LOCAL_INSTANCE
  $UTILS_DIR_BIN/top2.$LOCAL_INSTANCE -d 1 -b | grep Cpu > $HAT_TOP2_DATA_RAW.$LOCAL_INSTANCE &
  # Give sometime to stabilize power measurements
  echo -e "\nSleep time : ${SLEEP_TIME} second(s)" && sleep $SLEEP_TIME
  echo "-> PPC Tag <-"

elif [ "$LOCAL_COMMAND" = "stop" ]; then

  # Give sometime to stabilize power measurements
  echo -e "\nSleep time : ${SLEEP_TIME} second(s)" && sleep $SLEEP_TIME
  # Stop top2 data logging
  echo "-> PPC Tag <-"
  killall top2.$LOCAL_INSTANCE

elif [ "$LOCAL_COMMAND" = "parse" ]; then

  LOCAL_CPU_FIELD=$3

  echo -e "\nInfo: $LOCAL_CPU_FIELD CPU Usage"

  # See what numbers are requested, System or User
  if [ "$LOCAL_CPU_FIELD" = "User" ]; then
    cat $HAT_TOP2_DATA_RAW.$LOCAL_INSTANCE | sed -e 's/Cpu(s)://g' -e 's/[ ,a-z]//g' | cut -d% -f1 > $HAT_TOP2_DATA_TEMPORAL.$LOCAL_INSTANCE
  elif [ "$LOCAL_CPU_FIELD" = "System" ]; then
    cat $HAT_TOP2_DATA_RAW.$LOCAL_INSTANCE | sed -e 's/Cpu(s)://g' -e 's/[ ,a-z]//g' | cut -d% -f2 > $HAT_TOP2_DATA_TEMPORAL.$LOCAL_INSTANCE
  else
    echo "Ups! We cannot figure out what field you are requesting"
  fi

  # If debug enabled then print all raw results logged for CPU Usage
  if [ "$DEBUG" = "y" ]; then
    echo -e "Displaying File: Raw Format"
    cat $HAT_TOP2_DATA_RAW.$LOCAL_INSTANCE && echo
  fi

  getAverage
  getMaximum
  getMinimum

  logIt "Info: CPU Usage $LOCAL_CPU_FIELD [ Average: $average | Max: $max | Min: $min ]"

  # Clean Up
  rm $HAT_TOP2_DATA_TEMPORAL.$LOCAL_INSTANCE

fi

# End of file
