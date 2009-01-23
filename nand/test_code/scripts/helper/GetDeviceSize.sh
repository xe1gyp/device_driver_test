#!/bin/sh

device=$1
temp=`echo $device | cut -d ' ' -f2`
temp=`echo $temp | tr "a-z" "A-Z"`
TEMP_SIZE=`echo "ibase=16; ${temp}" | bc`
echo $TEMP_SIZE
