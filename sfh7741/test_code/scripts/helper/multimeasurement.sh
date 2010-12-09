#!/bin/bash

echo -e "Setting Multi Measurement Mode for Digital Compass:\n"
echo $DIGITAL_COMPASS_MULTI_MODE > $DIGITAL_COMPASS_OM

COUNT=0
while [ $COUNT -lt 100 ]
do
	for i in `ls $DIGITAL_COMPASS_ALL_AXIS`
	do
        	echo $i `cat $i`
		let COUNT++;
	done
	echo -e "-----------------\n"
done

if [ $? -eq 0 ]
then
	echo -e "PASS: Multi Measurement Test\n"
else
	echo -e "FAIL: Multi Measurement Test\n"
fi
