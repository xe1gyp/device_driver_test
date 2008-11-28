#!/bin/sh

# Input arguments
TEST_NUMBER=$1
TEST_REPOSITORY=$2

# Errors counter
ERRORS=0

# Test every channel with every buffer size
for CHANNELS in $CHANNELS_ARRAY
do
	$TESTSCRIPT/dma_endian.sh $TEST_NUMBER $CHANNELS $TEST_REPOSITORY
	$TESTSCRIPT/analyze_results.sh
	ERRORS=$(echo $ERRORS+$?|bc)
done

echo "*****************************************************************************"
echo "                                  SUMMARY                                    "
echo "*****************************************************************************"                                                                   
echo "Errors found: $ERRORS"
echo "*****************************************************************************"                                                                   

exit $ERRORS
