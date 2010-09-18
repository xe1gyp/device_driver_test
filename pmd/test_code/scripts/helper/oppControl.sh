#!/bin/sh


# =============================================================================
# Variables
# =============================================================================

LOCAL_COMMAND=$1
LOCAL_VDD=$2
LOCAL_OPP=$3

# =============================================================================
# Functions
# =============================================================================

# None

# =============================================================================
# Main
# =============================================================================

handlerError.sh "test"
if [ $? -eq 1 ]; then
	exit 1
fi

if [ ! -f $SYSFS_CPU0_CURRENT_FREQUENCY ]; then
	echo "FATAL: $SYSFS_CPU0_CURRENT_FREQUENCY cannot be found!"
	handlerError.sh "log" "1" "halt" "oppControl.sh"
	exit 1
fi

if [ ! -f $SYSFS_CPU0_SET_SPEED ]; then
	echo "FATAL: $SYSFS_CPU0_SET_SPEED cannot be found!"
	handlerError.sh "log" "1" "halt" "oppControl.sh"
	exit 1
fi


if [ ! -f $DSP_FREQ ]; then
	echo "FATAL: $DSP_FREQ cannot be found!"
	handlerError.sh "log" "1" "halt" "oppControl.sh"
	exit 1
fi

if [ "$LOCAL_COMMAND" = "show" ]; then

  if [ "$LOCAL_VDD" = "VDD1" ]; then
    cat $SYSFS_CPU0_CURRENT_FREQUENCY
  elif [ "$LOCAL_VDD" = "VDD2" ]; then
    cat $DSP_FREQ
  fi

elif [ "$LOCAL_COMMAND" = "request" ]; then

  if [ "$LOCAL_VDD" = "VDD1" ]; then
    echo "$LOCAL_OPP > $SYSFS_CPU0_SET_SPEED"
    echo $LOCAL_OPP > $SYSFS_CPU0_SET_SPEED
    temp_value=`cat $SYSFS_CPU0_CURRENT_FREQUENCY`
  elif [ "$LOCAL_VDD" = "VDD2" ]; then
	echo "Setting corresponding mpu frequency"
  	if [ "$LOCAL_OPP" = "260000000" ]; then
		echo "$VDD1_OPP1_FREQ > $SYSFS_CPU0_SET_SPEED"
		echo $VDD1_OPP1_FREQ > $SYSFS_CPU0_SET_SPEED
	elif [ "$LOCAL_OPP" = "520000000" ]; then
		echo "$VDD1_OPP2_FREQ > $SYSFS_CPU0_SET_SPEED"
		echo $VDD1_OPP2_FREQ > $SYSFS_CPU0_SET_SPEED
	elif [ "$LOCAL_OPP" = "660000000" ]; then
		echo "$VDD1_OPP3_FREQ > $SYSFS_CPU0_SET_SPEED"
		echo $VDD1_OPP3_FREQ > $SYSFS_CPU0_SET_SPEED
	elif [ "$LOCAL_OPP" = "800000000" ]; then
		echo "$VDD1_OPP4_FREQ > $SYSFS_CPU0_SET_SPEED"
		echo $VDD1_OPP4_FREQ > $SYSFS_CPU0_SET_SPEED
	fi
	temp_value=`cat $DSP_FREQ`
  fi

  if [ "$LOCAL_OPP" = "$temp_value" ]; then
    echo "OPP value was correctly set in $LOCAL_VDD"
  else
    echo "Error: OPP value $LOCAL_OPP was not set properly"
    echo "value found is $temp_value"
    exit 1
  fi

 elif [ "$LOCAL_COMMAND" = "stress" ]; then
   maxcount=40
   count=1

   while [ "$count" -le $maxcount ]
   do
       vdd1_opp_no=`expr $count % 4`
       vdd1_opp_no=`expr $vdd1_opp_no + 1`
       if [ "$vdd1_opp_no" = "1" ]; then
               LOCAL_OPP=300000
               echo $LOCAL_OPP > $SYSFS_CPU0_SET_SPEED
       elif [ "$vdd1_opp_no"  = "2" ]; then
               LOCAL_OPP=600000
               echo $LOCAL_OPP > $SYSFS_CPU0_SET_SPEED
       elif [ "$vdd1_opp_no" = "3" ]; then
               LOCAL_OPP=800000
               echo $LOCAL_OPP > $SYSFS_CPU0_SET_SPEED
       elif [ "$vdd1_opp_no" = "4" ]; then
               LOCAL_OPP=1000000
               echo $LOCAL_OPP > $SYSFS_CPU0_SET_SPEED
       fi

       temp_value=`cat $SYSFS_CPU0_CURRENT_FREQUENCY`

       if [ "$LOCAL_OPP" = "$temp_value" ]; then
               echo "OPP value was correctly set in $LOCAL_VDD"
       else
               echo "Error: OPP value $LOCAL_OPP was not set properly"
	       echo "value found is $temp_value"
       fi

       count=`expr $count + 1`
   done

else
  echo "Command in oppControl is not supported"
  exit 1
fi

# End of file
