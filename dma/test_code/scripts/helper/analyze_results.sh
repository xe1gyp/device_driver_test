#!/bin/sh

echo "Starting results analysis"
RESULTS_FILES=`cat $TMPBASE/results_files`

for FILE in $RESULTS_FILES
do
	echo "Stat_analyzer on $FILE"
	$TESTBIN/stat_analyzer $FILE || exit 1
	rm $FILE
done
