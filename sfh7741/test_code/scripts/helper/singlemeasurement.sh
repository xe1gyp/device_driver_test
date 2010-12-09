#!/bin/bash

echo -e "Setting Single Measurement Mode for Digital Compass:\n"
echo $DIGITAL_COMPASS_SINGLE_MODE > $DIGITAL_COMPASS_OM

for i in `ls $DIGITAL_COMPASS_ALL_AXIS`
do
	echo $i `cat $i`
done

echo -e "Setting Multi Measurement Mode for Digital Compass:\n"
echo $DIGITAL_COMPASS_MULTI_MODE > $DIGITAL_COMPASS_OM

