#!/bin/sh
set -x

LOCAL_COMMAND=$1

if [ "$LOCAL_COMMAND" = "mount" ]; then
  if [ -d $DEBUGFS_DIRECTORY ]
  then
    echo "Debug directory is already created"
  else
    mkdir $DEBUGFS_DIRECTORY
  fi
  
  mount | grep debugfs
  
  if [ $? != 0 ]; then
    mount -t debugfs debugfs /debug 
  fi
  
  
elif [ "$LOCAL_COMMAND" = "umount" ]; then
  umount $DEBUGFS_DIRECTORY
  rmdir $DEBUGFS_DIRECTORY

elif [ "$LOCAL_COMMAND" = "cat" ]; then
  cat $DEBUGFS_PMCOUNT

elif [ "$LOCAL_COMMAND" = "log" ]; then
  LOCAL_POWER_DOMAIN=$2
  LOCAL_HIT=$3
  LOCAL_VALUE=$4
  HIT_PLACE=0
  
  if [ "$LOCAL_HIT" = "OFF" ]; then
    HIT_PLACE=2
  elif [ "$LOCAL_HIT" = "RET" ]; then
    HIT_PLACE=3
  elif [ "$LOCAL_HIT" = "INA" ]; then
    HIT_PLACE=4
  elif [ "$LOCAL_HIT" = "ON" ]; then
    HIT_PLACE=5
  fi
  
  TEMP=`cat $DEBUGFS_PMCOUNT | grep $LOCAL_POWER_DOMAIN | grep $LOCAL_HIT`
  set $TEMP
  RESULT=`echo $2 | cut -d "," -f $HIT_PLACE`
  echo $RESULT > $TMPBASE.$LOCAL_VALUE

elif [ "$LOCAL_COMMAND" = "compare" ]; then
  LOCAL_VALUE_ONE=$2
  LOCAL_VALUE_TWO=$3
  FIRST=`cat $TMPBASE.$LOCAL_VALUE_ONE`
  SECOND=`cat $TMPBASE.$LOCAL_VALUE_TWO`

  cat $TMPBASE.$LOCAL_VALUE_ONE
  cat $TMPBASE.$LOCAL_VALUE_TWO
  
  diff $TMPBASE.$LOCAL_VALUE_ONE $TMPBASE.$LOCAL_VALUE_TWO
  
  if [ $? -eq 0 ]; then
    echo "Error: values match"
    exit 1
  fi

else
  echo "Command in debugFileSystem is not supported"
  exit 1
fi

# End of file