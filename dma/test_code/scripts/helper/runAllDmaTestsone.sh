# /vobs/wtbu/OMAPSW_L/linux/test/device_driver_test/dma/test_code/runAllDmaTests.sh
#
# Test case automation for DMA testing
# params - testNumbers to run
# 1- aync transfer tests - no chaining
# 2- chaining of every channel and loop to every channel less than last channel
# 3- chain all channels till an intermediate channel and loop to zero, rest are unchained
# 4- chain all channels from an intermediate channel and loop to middlechannel, rest are unchained
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
IMAGE=./dmatest.ko
else
IMAGE=./dmatest.o
fi
IMAGE_NAME=dmatest
MAX_CHANNELS=16
PROCFILE=/proc/driver/dmatest

# Grab TCs to run
while [ $# -gt 0 ]
do
	case "$1" in
	1)
		test1=1
		echo "TEST1"
		;;
	2)
		test2=1
		echo "TEST2"
		;;
	3)
		test3=1
		echo "TEST3"
		;;
	esac
	shift
done

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
	insmod -f $IMAGE $* 
	echo "Sleeping 2 seconds(allow transfers to complete)"
	sleep 2
	echo "Test params were:"
	cat $PROCFILE
	echo "Removing module"
	rmmod $IMAGE_NAME
	
}

TESTNUM=1
curChannelCount=1
if [ x"$test1" = x1 ]; then
	HeadLine "Async channel transfer testswith endian conversion"
	while [ $curChannelCount -le $MAX_CHANNELS ]
	do
		testdma $TESTNUM channels=$curChannelCount
		curChannelCount=`expr $curChannelCount + 1`
		TESTNUM=`expr $TESTNUM + 1`
	done
fi

if [ x"$test2" = x1 ]; then
	# Linked High priority thread Loop test Link to each of the non 0th channel
	HeadLine "All single channels started off.. and then priority tests with endian conversion"
	curChannelCount=2
	while [ $curChannelCount -le $MAX_CHANNELS ]
	do

		#Chained transfers after linking
		testdma $TESTNUM channels=$curChannelCount prio=1
		TESTNUM=`expr $TESTNUM + 1`
		curChannelCount=`expr $curChannelCount + 1`
	done
fi
if [ x"$test3" = x1 ]; then
	# Linked High priority thread Loop test Link to each of the non 0th channel
	HeadLine "check if we can query data with endian conversion"
	curChannelCount=2
	while [ $curChannelCount -le $MAX_CHANNELS ]
	do

		#Chained transfers after linking
		testdma $TESTNUM channels=$curChannelCount query_idx=1
		TESTNUM=`expr $TESTNUM + 1`
		curChannelCount=`expr $curChannelCount + 1`
	done
fi
