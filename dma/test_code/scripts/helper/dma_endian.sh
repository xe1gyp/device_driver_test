#!/bin/sh

# General conditions
EXIT_SUCCESSFUL=0
EXIT_FAIL=1
EXT=ko
PROCFILE=/proc/driver/dmatest
CHANNELS_DEFAULT=16
REPOSITORY_DEFAULT='/tmp'	# Default repository. Use only if this script is executed out of the testsuite.
MODS_DIR=$TESTMODS

# Enable if INSMOD fails for kernel name length
#FORCE="-f"

# Print headline
HeadLine()
{
        echo "*****************************************************************************"
        echo $*
        echo "*****************************************************************************"
}

# Parse input arguments
ParseInput()
{
	case $# in
		1)	TEST_NUMBER=$1
			CHANNELS=$CHANNELS_DEFAULT
			REPOSITORY=$REPOSITORY_DEFAULT
			echo "*****************************************************************************"
	                echo "Test Number: $TEST_NUMBER"
	                echo "Channels: $CHANNELS (Default value)"
			echo "Repository: $REPOSITORY (Default value)"
			echo "*****************************************************************************"
			;;
		2)	TEST_NUMBER=$1
                        CHANNELS=$2
			REPOSITORY=$REPOSITORY_DEFAULT
			echo "*****************************************************************************"
                        echo "Test Number: $TEST_NUMBER"
                        echo "Channels: $CHANNELS"
			echo "Repository: $REPOSITORY (Default value)"
			echo "*****************************************************************************"
			;;
                3)      TEST_NUMBER=$1
                        CHANNELS=$2
                        REPOSITORY=$3
			echo "*****************************************************************************"
                        echo "Test Number: $TEST_NUMBER"
                        echo "Channels: $CHANNELS"
                        echo "Repository: $REPOSITORY"
			echo "*****************************************************************************"
                        ;;
                *)      echo "Usage: dma_endian.sh TEST_NUMBER CHANNELS REPOSITORY"
			exit $EXIT_FAILURE
                        ;;
	esac

	# Export parameters
	export TEST_NUMBER
	export CHANNELS
	export REPOSITORY
}

#  channels - Number of channels to use
#  linking  - Whether to chain or not (if not !=0 then linking points to the channel number to link
#             the last channel to)
#  nochain  -  0 - No non-chained channels
#              1 - Do chaining after the linking channel
#             -1 - Do chaining before the linking channel (rest of the channels are un-chained)
TestDMA()
{
	NUM=$1
	shift
	MOD_NAME=$1
	shift
	echo "Module: $MOD_NAME"
	echo "Params: $*"

	echo "Inserting Module"
	insmod $FORCE $MODS_DIR/$MOD_NAME.$EXT $* 
	echo "Sleeping 2 seconds (allow transfers to complete)"
	sleep 2

	echo "Test results"
	echo "Proc file: $PROCFILE"
	cp $PROCFILE $REPOSITORY/$MOD_NAME

        echo "Removing module"
        rmmod $MOD_NAME
}

Generic()
{
	MOD_NAME=$1
	shift
	NUM=$CHANNELS

	echo "=== Testing Module: $MOD_NAME - Channels: $CHANNELS ==="
	TestDMA $NUM $MOD_NAME channels=$CHANNELS $*
	rm -f $FILE
}

Waitup()
{
	TestNum=$1;
	while [ -f $TMP_FILE1 -o -f $TMP_FILE2 -o -f $TMP_FILE3 ]
	do
		echo "Test $TestNum - Waiting..."
		sleep 4
	done
	echo "Test $TestNum - Wait routine: Done"
}

SelectRunTest()
{
	case $TEST_NUMBER in
		1)	# Async channel transfer tests with endian conversion
			HeadLine "Async Channel with Endian Conversion"
			Generic dmatest
                        rm $TMPBASE/results_files
                        echo "$TMPBASE/dmatest" >> $TMPBASE/results_files
			;;
                2)	# Async channel transfer with priority and endian conversion
                        HeadLine "Async Channel with Priority and Endian Conversion"
                        Generic dmatest prio=4
                        rm $TMPBASE/results_files
                        echo "$TMPBASE/dmatest" >> $TMPBASE/results_files
			;;
                3)	# Async channel transfer tests with endian conversion
                        HeadLine "Async Channel with Query and Endian Conversion"
                        Generic dmatest query_idx=1
                        rm $TMPBASE/results_files
                        echo "$TMPBASE/dmatest" >> $TMPBASE/results_files
			;;
		*)	# Invalid test numbers
			echo "Invalid test number"
	esac
}


# =================
#       Main
# =================

# Parse all input arguments
ParseInput $@

# Select and run test
SelectRunTest
