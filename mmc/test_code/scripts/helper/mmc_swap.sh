#!/bin/bash

echo -e "Checking for existing swap files / partitions\n"

SWAP_EXIT=`free | grep Swap | cut -c18,20`
echo $SWAP_EXIT

if [ $SWAP_EXIT -eq 0 ];
then
	echo -e "PASS: No swap files found, OK\n"
else
	echo -e "FAIL: Swap files found !!\n"
	exit 1
fi

echo -e "Displaying current /proc/swaps info\n"
cat $PROCFS_SWAPS

dd if=/dev/zero of=$SWAP_FILENAME bs=1M count=$SWAP_FILENAME_SIZE

if [ $? -eq 0 ]
then
	cd /
	echo -e "Formating SWAP partition file:" $SWAP_FILENAME "\n"
	mkswap $SWAP_FILENAME
	echo -e "Setting up SWAP partition:\n"
	swapon -p $SWAP_FILENAME_PRIORITY $SWAP_FILENAME
	echo -e "Now, $SWAP_FILENAME should be shown at $PROCFS_SWAPS \n"
	cat $PROCFS_SWAPS
	echo -e "Verifying that $SWAP_FILENAME was created properly\n"
	strings -a $SWAP_FILENAME | grep $SWAP_FILENAME_GREP
	echo -e "Unloading SWAP file ..."
	swapoff $SWAP_FILENAME
	exit 0
else
	echo -e "FATAL, SWAP file can't be created...\n"
	exit 1
fi

SWAP_EXIT2=`free | grep Swap | cut -c18,20`

if [ $SWAP_EXIT2 -eq 0 ]
then
        echo -e "PASS: No swap files found, OK\n"
else
        echo -e "FAIL: Swap files found !!\n"
        exit 1
fi


