#!/bin/sh

device=$1
temp=`echo "${device}" | cut -d ' ' -f3`
temp=`echo $temp | tr "a-z" "A-Z"`
TEMP_ERASESIZE=`echo "ibase=16; ${temp}" | bc`
echo $TEMP_ERASESIZE
