#!/bin/sh
set -x

LOCAL_COMMAND=$1

if [ "$LOCAL_COMMAND" = "list" ]; then
  cat $SCALING_AVAILABLE_GOVERNORS
elif [ "$LOCAL_COMMAND" = "set" ]; then
  LOCAL_GOVERNOR=$2
  if [ "$LOCAL_GOVERNOR" = "all" ]; then
    error=0
    echo > $GOVERNORS_LIST_OK
    echo > $GOVERNORS_LIST_ERROR

    available_governors=`cat $SCALING_AVAILABLE_GOVERNORS`

    for i in $available_governors
    do
      echo "Setting Governor to" $i
      echo "echo $i > $SCALING_GOVERNOR"
      echo $i > $SCALING_GOVERNOR
      if [ $? != 0 ]; then
        echo "Error: Governor $i cannot be set"
        echo $i >> $GOVERNORS_LIST_ERROR
        error=1
      else
        echo "Governor $i correctly set"
        echo $i >> $GOVERNORS_LIST_OK
      fi
      sleep 1
    done

    echo "The following governors were correctly set"
    cat $GOVERNORS_LIST_OK
    echo "The following governors couldn't be set"
    cat $GOVERNORS_LIST_ERROR
    sleep 5
    if [ $error -eq 1 ]; then
      exit 1
    fi

  else
    echo $LOCAL_GOVERNOR > $SCALING_GOVERNOR
    if [ $? != 0 ]; then
      echo "Error: Governor $i cannot be set"
      exit 1
    else
      echo "Governor $LOCAL_GOVERNOR correctly set"
    fi
    sleep 1
  fi
else
  echo "Command in cpuFrequencyScaling is not supported"
  exit 1
fi

# End of file