#!/bin/sh
set -x

LOCAL_COMMAND=$1
LOCAL_VDD=$2

if [ "$LOCAL_COMMAND" = "enable" ]; then
  
  if [ "$LOCAL_VDD" = "VDD1" ]; then
    echo "echo $ENABLE > $SR_VDD1_AUTOCOMP"
    echo $ENABLE > $SR_VDD1_AUTOCOMP
  elif [ "$LOCAL_VDD" = "VDD2" ]; then
    echo "echo $ENABLE > $SR_VDD2_AUTOCOMP"
    echo $ENABLE > $SR_VDD2_AUTOCOMP
  fi
   
elif [ "$LOCAL_COMMAND" = "disable" ]; then

  if [ "$LOCAL_VDD" = "VDD1" ]; then
    echo "echo $ENABLE > $SR_VDD1_AUTOCOMP"
    echo $DISABLE > $SR_VDD1_AUTOCOMP
  elif [ "$LOCAL_VDD" = "VDD2" ]; then
    echo "echo $ENABLE > $SR_VDD2_AUTOCOMP"
    echo $DISABLE > $SR_VDD2_AUTOCOMP
  fi

else
  echo "Command in smartReflexAutoComp is not supported"
  exit 1
fi

# End of file