#!/bin/bash

echo -e "Setting Multi Measurement Mode for Digital Compass:\n"
echo $DIGITAL_COMPASS_MULTI_MODE > $DIGITAL_COMPASS_OM

$UTILS_DIR_SCRIPTS/dvfs.opp.sh

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
        echo -e "PASS: DVFS Test\n"
else
        echo -e "FAIL: DVFS Test\n"
fi

$UTILS_DIR_SCRIPTS/fixed.opp.sh &

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
        echo -e "PASS: OPP Test\n"
else
        echo -e "FAIL: OPP Test\n"
fi

