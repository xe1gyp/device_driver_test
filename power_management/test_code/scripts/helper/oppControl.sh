#!/bin/sh
set -x

LOCAL_COMMAND=$1
LOCAL_VDD=$2
LOCAL_OPP=$3

if [ "$LOCAL_COMMAND" = "show" ]; then

  if [ "$LOCAL_VDD" = "VDD1" ]; then
    cat $VDD1_OPP
  elif [ "$LOCAL_VDD" = "VDD2" ]; then
    cat $VDD2_OPP
  fi

elif [ "$LOCAL_COMMAND" = "request" ]; then
  
  if [ "$LOCAL_VDD" = "VDD1" ]; then
    echo "$LOCAL_OPP > $VDD1_OPP"
    echo $LOCAL_OPP > $VDD1_OPP
    temp_value=`cat $VDD1_OPP`
  elif [ "$LOCAL_VDD" = "VDD2" ]; then
    echo "echo $LOCAL_OPP > $VDD2_OPP"
    echo $LOCAL_OPP > $VDD2_OPP
    temp_value=`cat $VDD2_OPP`
  fi
  
  if [ "$LOCAL_OPP" = "$temp_value" ]; then
    echo "OPP value was correctly set in $LOCAL_VDD"
  else
    echo "Error: OPP value $LOCAL_OPP was not set properly, value found is $temp_value"
    exit 1
  fi
    
elif [ "$LOCAL_COMMAND" = "lock" ]; then

  if [ "$LOCAL_VDD" = "VDD1" ]; then
    echo "echo $LOCAL_OPP > $VDD1_LOCK" 
    echo $LOCAL_OPP > $VDD1_LOCK
  elif [ "$LOCAL_VDD" = "VDD2" ]; then
    echo "echo $LOCAL_OPP > $VDD2_LOCK" 
    echo $LOCAL_OPP > $VDD2_LOCK
  fi
  
  if [ $? != 0 ]; then
    echo "Error: $LOCAL_VDD couldn't be locked with value $LOCAL_OPP"
    exit 1
  fi


elif [ "$LOCAL_COMMAND" = "stress" ]; then
  maxcount=99
  count=3

  while [ "$count" -le $maxcount ]
  do
  	vdd1_opp_no=`expr $count % 5`	
  	vdd1_opp_no=`expr $vdd1_opp_no + 1`
  	echo -n $vdd1_opp_no > $VDD1_OPP
  	echo VDD1:
  	cat $VDD1_OPP
  	vdd2_opp_no=`expr $count % 2`
  	vdd2_opp_no=`expr $vdd2_opp_no + 2`
  	echo -n $vdd2_opp_no > $VDD2_OPP
  	echo VDD2:
  	cat $VDD2_OPP
  	count=`expr $count + 1`
  done
  
elif [ "$LOCAL_COMMAND" = "stress_invalid_opp" ]; then
  maxcount=99

  if [ "$LOCAL_VDD" = "VDD1" ]; then
	  count=5
	  while [ "$count" -le $maxcount ]
	  do
	  	vdd1_opp_no=`expr $count + 1`	
	  	echo -n $vdd1_opp_no > $VDD1_OPP
	  	echo VDD1:
	  	cat $VDD1_OPP
		temp_value=`cat $VDD1_OPP`
		if [ "$vdd1_opp_no" = "$temp_value" ]; then
		    echo "Error: OPP value was set in $vdd1_opp_no"
	    	    exit 1
		  else
		    echo "OPP value $vdd1_opp_no was not set properly, as expected"
		fi
	  	count=`expr $count + 1`
	   done

   elif [ "$LOCAL_VDD" = "VDD2" ]; then
	 count=3
	 while [ "$count" -le $maxcount ]
	  do
	  	vdd2_opp_no=`expr $count + 1`
	  	echo -n $vdd2_opp_no > $VDD2_OPP
	  	echo VDD2:
	  	cat $VDD2_OPP
		temp_value=`cat $VDD2_OPP`
		if [ "$vdd2_opp_no" = "$temp_value" ]; then
		    echo "Error: OPP value was set in $vdd2_opp_no"
	    	    exit 1
		  else
		    echo "OPP value $vdd2_opp_no was not set properly, as expected"
		fi
  		count=`expr $count + 1`
	  done
   fi

else
  echo "Command in oppControl is not supported"
  exit 1
fi

# End of file
