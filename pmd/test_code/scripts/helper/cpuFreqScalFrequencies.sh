#!/bin/sh
set -x

LOCAL_COMMAND=$1

# Checking if userspace governor is available
cat $SCALING_AVAILABLE_GOVERNORS | grep $GOVERNOR_USERSPACE
if [ $? -eq 1 ]; then
  echo "Error: $GOVERNOR_USERSPACE governor is not available, cannot continue"
  exit 1
fi

# Checking if scaling_setspeed has been created
if [ ! -e $SCALING_SET_SPEED ]
then
	echo "Fatal: $SCALING_SET_SPEED is not available, cannot continue"
	exit 1
fi

# Changing to userspace governor
cpuFreqScalGovernors.sh set $GOVERNOR_USERSPACE
if [ $? != 0 ]; then
  echo "Error: $GOVERNOR_USERSPACE governor cannot be set, cannot continue"
  exit 1
fi

if [ "$LOCAL_COMMAND" = "list" ]; then
  cat $SCALING_AVAILABLE_FREQUENCIES
elif [ "$LOCAL_COMMAND" = "set" ]; then
  
  # Changing through all the frequencies available
  available_frequencies=`cat $SCALING_AVAILABLE_FREQUENCIES`
  
  for i in $available_frequencies
	 do
  		echo "Setting Frequency to " $i
  		echo "echo $i > $SCALING_SET_SPEED"
  		echo $i > $SCALING_SET_SPEED
  		cur_frequency=`cat $SCALING_CUR_FREQ`
  		if [ "$i" != "$cur_frequency" ]
  		then
  			echo "Fatal: Current frequency is different from the set one"
  			exit 1
  		fi
  done

else
  echo "Command in cpuFrequencyScaling is not supported"
  exit 1
fi

# End of file