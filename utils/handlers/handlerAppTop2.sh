#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_COMMAND=$1
Top2Scale=0

# =============================================================================
# Functions
# =============================================================================

logIt() {

  echo -e $1
  echo -e $1 >> $CPU_LOG_FILE

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
  done < $CPU_TEMPORAL_FILE
  average=`echo "scale=$Top2Scale; $acc/$n"|bc`
  logIt "Average Value: ${average}"
  echo $average > ${CPU_USAGE_VALUE_AVR}


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
  done < $CPU_TEMPORAL_FILE
  logIt "Maximum Value: ${max}"
  echo $max > ${CPU_USAGE_VALUE_MAX}

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
  done < $CPU_TEMPORAL_FILE
  logIt "Minimum Value: ${min}"
  echo $min > ${CPU_USAGE_VALUE_MIN}

}

# =============================================================================
# Main
# =============================================================================

if [ "$LOCAL_COMMAND" = "start" ]; then

  echo > $CPU_LOG_FILE

  # Start top2 data logging
  top2 -d 1 -b | grep Cpu > $CPU_USAGE_RESULTS &
  # Give sometime to stabilize power measurements
  echo -e "\nSleep time : ${SLEEP_TIME} second(s)" && sleep $SLEEP_TIME
  echo "-> PPC Tag <-"

elif [ "$LOCAL_COMMAND" = "stop" ]; then

  # Give sometime to stabilize power measurements
  echo -e "\nSleep time : ${SLEEP_TIME} second(s)" && sleep $SLEEP_TIME
  # Stop top2 data logging
  echo "-> PPC Tag <-"
  killall top2

elif [ "$LOCAL_COMMAND" = "parse" ]; then

  LOCAL_CPU_FIELD=$2

  logIt "\nInfo: $LOCAL_CPU_FIELD CPU Usage\n"

  # See what numbers are requested, System or User
  if [ "$LOCAL_CPU_FIELD" = "User" ]; then
    cat $CPU_USAGE_RESULTS | sed -e 's/Cpu(s)://g' -e 's/[ ,a-z]//g' | cut -d% -f1 > $CPU_TEMPORAL_FILE
  elif [ "$LOCAL_CPU_FIELD" = "System" ]; then
    cat $CPU_USAGE_RESULTS | sed -e 's/Cpu(s)://g' -e 's/[ ,a-z]//g' | cut -d% -f2 > $CPU_TEMPORAL_FILE
  else
    echo "Ups! We cannot figure out what field you are requesting"
  fi

  # If debug enabled then print all raw results logged for CPU Usage
  if [ "$DEBUG" = "y" ]; then
    echo -e "Displaying File: Raw Format"
    cat $CPU_USAGE_RESULTS && echo
  fi

  getAverage
  getMaximum
  getMinimum

  logIt "\nInfo: $LOCAL_CPU_FIELD CPU Usage [ Average: $average | Max: $max | Min: $min ]\n"

  # Clean Up
  rm $CPU_TEMPORAL_FILE

fi

# End of file
