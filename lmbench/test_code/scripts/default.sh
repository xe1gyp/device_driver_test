#!/bin/sh

cd ../utils/lmbench-3.0-a9/scripts/
echo -e "1\n 1\n $TEST_MB\n all\n \n \n \n \n \n \n \n no\n" |./config-run
./results
echo "==========================================================================================="
echo "Search the results file SDP3430-3430.0 or .X depends of the times that you run the test \n"
echo "it's located in ../utils/lmbench-3.0-a9/results/
echo "==========================================================================================="
