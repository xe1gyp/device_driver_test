#!/bin/sh
set -x

LOCAL_COMMAND=$1
LOCAL_VDD=$2
LOCAL_OPP=$3

if [ "$LOCAL_COMMAND" = "show" ]; then

  if [ "$LOCAL_VDD" = "VDD1" ]; then
    cat $SCALING_CUR_FREQ
  elif [ "$LOCAL_VDD" = "VDD2" ]; then
    cat $DSP_FREQ
  fi

elif [ "$LOCAL_COMMAND" = "request" ]; then

  if [ "$LOCAL_VDD" = "VDD1" ]; then
    echo "$LOCAL_OPP > $SCALING_SET_SPEED"
    echo $LOCAL_OPP > $SCALING_SET_SPEED
    temp_value=`cat $SCALING_CUR_FREQ`
  elif [ "$LOCAL_VDD" = "VDD2" ]; then
	echo "Setting corresponding mpu frequency"
  	if [ "$LOCAL_OPP" = "260000000" ]; then
		echo "$VDD1_OPP1_FREQ > $SCALING_SET_SPEED"
		echo $VDD1_OPP1_FREQ > $SCALING_SET_SPEED
	elif [ "$LOCAL_OPP" = "520000000" ]; then
		echo "$VDD1_OPP2_FREQ > $SCALING_SET_SPEED"
		echo $VDD1_OPP2_FREQ > $SCALING_SET_SPEED
	elif [ "$LOCAL_OPP" = "660000000" ]; then
		echo "$VDD1_OPP3_FREQ > $SCALING_SET_SPEED"
		echo $VDD1_OPP3_FREQ > $SCALING_SET_SPEED
	elif [ "$LOCAL_OPP" = "800000000" ]; then
		echo "$VDD1_OPP4_FREQ > $SCALING_SET_SPEED"
		echo $VDD1_OPP4_FREQ > $SCALING_SET_SPEED
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

else
  echo "Command in oppControl is not supported"
  exit 1
fi

# End of file
