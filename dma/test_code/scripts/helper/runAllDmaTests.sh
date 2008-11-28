# /vobs/wtbu/OMAPSW_L/linux/test/device_driver_test/dma/test_code/runAllDmaTests.sh
#
# Test case automation for DMA testing
# params - testNumbers to run
# Test Description:
# TN-Test Description
# 1- Unchained
# 2- Static Chained
# 3- Dynamic  Chained
# 4- Dynamic  Chained + unchained
# 5- Static Chained + Unchained
# 6- Dynamic + Static Chained
# 7- Dynamic + Static Chained + unchained
# 8- UnChained priority enable
# 9- UnChained query index
#
# Copyright (C) 2005 Texas Instruments, Inc.
#
# This package is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
# WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
#


if [ -f .26 ]; then
EXT=ko
else
EXT=o
fi
TMP_FILE1=/tmp/file.1.$$
TMP_FILE2=/tmp/file.2.$$
TMP_FILE3=/tmp/file.3.$$

PRE_REQ="dma_buf_logic dma_stat"

MAX_CHANNELS=16
# Each transfer with 512 K buffer per channel under test
BUFFER_SIZE=32768
PROCFILE=/proc/driver/dmatest
#---- ENABLE THIS IF INSMOD FAILS FOR KERNEL NAME LENGTH
#FORCE="-f"

TESTS_TO_RUN=""

# Grab TCs to run
while [ $# -gt 0 ]
do
	case "$1" in
	1)
		test1=1
		TESTS_TO_RUN="$TESTS_TO_RUN TEST1"
		;;
	2)
		test2=1
		TESTS_TO_RUN="$TESTS_TO_RUN TEST2"
		;;
	3)
		test3=1
		TESTS_TO_RUN="$TESTS_TO_RUN TEST3"
		;;
	4)
		test4=1
		TESTS_TO_RUN="$TESTS_TO_RUN TEST4"
		;;
	5)
		test5=1
		TESTS_TO_RUN="$TESTS_TO_RUN TEST5"
		;;
	6)
		test6=1
		TESTS_TO_RUN="$TESTS_TO_RUN TEST6"
		;;
	7)
		test7=1
		TESTS_TO_RUN="$TESTS_TO_RUN TEST7"
		;;
	8)
		test8=1
		TESTS_TO_RUN="$TESTS_TO_RUN TEST8"
		;;
	9)
		test9=1
		TESTS_TO_RUN="$TESTS_TO_RUN TEST9"
		;;
	10)
		test10=1
		TESTS_TO_RUN="$TESTS_TO_RUN TEST10"
		;;
	11)
		test11=1
		TESTS_TO_RUN="$TESTS_TO_RUN TEST11"
		;;
	12)
		test12=1
		TESTS_TO_RUN="$TESTS_TO_RUN TEST12"
		;;

	esac
	shift
done

echo "Tests to be run: $TESTS_TO_RUN"

HeadLine()
{
	echo "*****************************************************************************"
	echo $*
	echo "*****************************************************************************"
}

#- Params 
#  channels = number of channels to use
#  linking = whether to chain or not (if not !=0 then linking points to the channel number to link
#                             the last channel to)
# nochain =  0 - no non-chained channels
#            1 - do chaining after the linking channel
#            -1 - do chaining before the linking channel (rest of the channels are un-chained)
testdma()
{
	echo "TEST NUMBER $1"
	shift
	echo "params=$*"
	echo "Inserting Module"
	insmod $FORCE $IMAGE_NAME.$EXT $* 
	echo "Sleeping 2 seconds(allow transfers to complete)"
	sleep 2
	echo "Test params were:"
	cat $PROCFILE$IMAGE_NAME
	echo "Removing module"
	rmmod $IMAGE_NAME
	
}

generic()
{
	export IMAGE_NAME=$1
	shift
	FILE=$1
	shift
	curChannelCount=1
	while [ $curChannelCount -le $MAX_CHANNELS ]
	do
		testdma $TESTNUM channels=$curChannelCount buf_size=$BUFFER_SIZE $*
		curChannelCount=`expr $curChannelCount + 1`
		TESTNUM=`expr $TESTNUM + 1`
	done
	rm -f $FILE
}
waitup()
{
	TestNum=$1;
	while [ -f $TMP_FILE1 -o -f $TMP_FILE2 -o -f $TMP_FILE3 ]
	do
		sleep 4
		echo "Waitin..Test $TestNum"
	done
	echo "Done waiting $TestNum"
}
for i in $PRE_REQ
do
insmod $FORCE $i.$EXT
done

TESTNUM=1
if [ x"$test1" = x1 ]; then
	# Unchained
	HeadLine "Unchained channel transfer tests"
	generic dmaunlnk $TMP_FILE1
fi

if [ x"$test2" = x1 ]; then
	# Static Chained
	HeadLine "Static chained Transfer "
	generic dmastatic_chain $TMP_FILE1
fi

if [ x"$test3" = x1 ]; then
	# Dynamic Chained
	HeadLine "Dynamic Chained Transfer"
	generic dmadynamic_chain $TMP_FILE1
fi

export MAX_CHANNELS=8
if [ x"$test4" = x1 ]; then
	# Dynamic  Chained + unchained
	HeadLine "Unchained Channel & Dynamic Chained Transfer"
	touch $TMP_FILE1
	touch $TMP_FILE2
	generic dmaunlnk $TMP_FILE1 &
	generic dmadynamic_chain $TMP_FILE2 
	waitup 4
fi

if [ x"$test5" = x1 ]; then
	# Static Chained + unchained
	HeadLine "Unchained Channel & Static Chained Transfer"
	touch $TMP_FILE1
	touch $TMP_FILE2
	generic dmaunlnk $TMP_FILE1 &
	generic dmastatic_chain $TMP_FILE2 
	waitup 5
fi

if [ x"$test6" = x1 ]; then
	# Dynamic + Static Chained
	HeadLine "Dynamic & Static Chained Transfer"
	touch $TMP_FILE1
	touch $TMP_FILE2
	generic dmadynamic_chain $TMP_FILE1 &
	generic dmastatic_chain $TMP_FILE2 
	waitup 6
fi

if [ x"$test7" = x1 ]; then
	# Dynmic + Static Chained + unchained
	HeadLine "Unchained + Dynamic + Static Chained Transfer"
	touch $TMP_FILE1
	touch $TMP_FILE2
	touch $TMP_FILE3
	generic dmadynamic_chain $TMP_FILE1 &
	generic dmaunlnk $TMP_FILE3 &
	generic dmastatic_chain $TMP_FILE2 
	waitup 7
fi

if [ x"$test8" = x1 ]; then
	# UnChained priority enable
	HeadLine "Priority Test"
	generic dmaunlnk $TMP_FILE1 prio=2
fi
if [ x"$test9" = x1 ]; then
	# UnChained query index
	HeadLine "Query Test"
	generic dmaunlnk $TMP_FILE1 query_idx=1
fi

if [ x"$test10" = x1 ]; then
	# UnChained burst mode
	HeadLine "Enable burst mode in unchained"
	generic dmaunlnk $TMP_FILE1 burst=1
fi
if [ x"$test11" = x1 ]; then
	# Static Chained
	HeadLine "Static chained + burst enable Transfer "
	generic dmastatic_chain $TMP_FILE1 burst=1
fi

if [ x"$test12" = x1 ]; then
	# Dynamic Chained
	HeadLine "Dynamic Chained Transfer + burst enable"
	generic dmadynamic_chain $TMP_FILE1 burst=1
fi



rm -f $TMP_FILE2 $TMP_FILE1 $TMP_FILE3

for i in $PRE_REQ
do
rmmod $i
done

