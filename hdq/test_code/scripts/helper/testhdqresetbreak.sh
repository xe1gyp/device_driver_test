#!/bin/sh

NAME=$1
x=16
LIMIT=255

echo "open" > $HDQ_SYSFS_CMD | grep "fault"

for i in "reset break"
do
  while [ $x -lt $LIMIT ]
  do
    x=`expr $x + 1`
    data=`printf "%X" $x`
    echo "write 01 $data" > $HDQ_SYSFS_CMD
    echo "read 01" > $HDQ_SYSFS_CMD
    echo "$i" > $HDQ_SYSFS_CMD
    rx=`cat $HDQ_SYSFS_RESULT`
    if [ "$rx" != "OK" ]
    then
      echo "Interaction $x <> Expected OK <> Received $rx <> FAIL"
      status=1
    else 
      echo "Interaction $x <> Expected OK <> Received $rx <> PASS"
    fi
  done
done
echo "close" > $HDQ_SYSFS_CMD

if [ "$status" -eq 1 ] ; then
  exit 1
else 
  exit 0
fi
