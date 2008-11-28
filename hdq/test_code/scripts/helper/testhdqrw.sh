#!/bin/sh

x=16
LIMIT=191
status=0

echo "open" > $HDQ_SYSFS_CMD | grep "fault"

while [ $x -lt $LIMIT ]
do
  x=`expr $x + 1`
  data=`printf "%X" $x`
  echo "read 01" > $HDQ_SYSFS_CMD
  echo "write 01 $data" > $HDQ_SYSFS_CMD
  echo "read 01" > $HDQ_SYSFS_CMD
  rx=`cat $HDQ_SYSFS_RESULT`
  if [ "$data" != "$rx" ]
  then
    echo "Transmitted $data <> Received $rx <> FAIL"
    status=1
  else 
    echo "Transmitted $data <> Received $rx <> PASS"
  fi
  
done

echo "close" > $HDQ_SYSFS_CMD

if [ "$status" -eq 1 ] ; then
  exit 1
else 
  exit 0
fi
