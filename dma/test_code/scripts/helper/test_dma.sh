#!/bin/sh

# Input arguments
TEST_NUMBER=$1
TEST_REPOSITORY=$2

# Errors counter
ERRORS=0

# Test every channel with every buffer size
for CHANNELS in $CHANNELS_ARRAY
do
	for BUFFER_SIZE in $BUFFER_SIZE_ARRAY
	do
		$TESTSCRIPT/dma.sh $TEST_NUMBER $CHANNELS $BUFFER_SIZE $TEST_REPOSITORY
		$TESTSCRIPT/analyze_results.sh
		ERRORS=$(echo $ERRORS+$?|bc)
	done
done

echo "*****************************************************************************"
echo "                                  SUMMARY                                    "
echo "*****************************************************************************"                                                                   
echo "Errors found: $ERRORS"
echo "*****************************************************************************"                                                                   

exit $ERRORS
