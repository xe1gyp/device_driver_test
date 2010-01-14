#!/bin/sh

test -f $TEXT_FILE_SAMPLE.temp && rm $TEXT_FILE_SAMPLE.temp
touch $TEXT_FILE_SAMPLE.temp

for i in $(seq 1 10)
do
	cat $TEXT_FILE_PATTERN >> $TEXT_FILE_SAMPLE.temp
done

for i in $(seq 1 500)
do
        cat $TEXT_FILE_SAMPLE.temp >> $TEXT_FILE_SAMPLE
done


# End of file
