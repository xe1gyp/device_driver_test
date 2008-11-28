#!/bin/sh

# Test case automation for DMA testing
# params - testNumbers to run
# Test Description:
# TN-Test Description
#  1 - Unchained
#  2 - Static Chained
#  3 - Dynamic Chained
#  4 - Dynamic Chained + unchained
#  5 - Static Chained + Unchained
#  6 - Dynamic + Static Chained
#  7 - Dynamic + Static Chained + unchained
#  8 - UnChained priority enable
#  9 - UnChained query index
# 10 - Enable burst mode in unchained
# 11 -
# 12 - 


# General conditions
EXIT_SUCCESSFUL=0
EXIT_FAIL=1
EXT=ko
TMP_FILE1=/tmp/file.1.$$
TMP_FILE2=/tmp/file.2.$$
TMP_FILE3=/tmp/file.3.$$
PROCFILE=/proc/driver/dmatest
PRE_REQ="dma_buf_logic dma_stat"
CHANNELS_DEFAULT=16
BUFFER_SIZE_DEFAULT=131072
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
			BUFFER_SIZE=$BUFFER_SIZE_DEFAULT
			REPOSITORY=$REPOSITORY_DEFAULT
			echo "*****************************************************************************"
	                echo "Test Number: $TEST_NUMBER"
	                echo "Channels: $CHANNELS (Default value)"
        	        echo "Buffer Size: $BUFFER_SIZE (Default value)"
			echo "Repository: $REPOSITORY (Default value)"
			echo "*****************************************************************************"
			;;
		2)	TEST_NUMBER=$1
                        CHANNELS=$2
                        BUFFER_SIZE=$BUFFER_SIZE_DEFAULT
			REPOSITORY=$REPOSITORY_DEFAULT
			echo "*****************************************************************************"
                        echo "Test Number: $TEST_NUMBER"
                        echo "Channels: $CHANNELS"
                        echo "Buffer Size: $BUFFER_SIZE (Default value)"
			echo "Repository: $REPOSITORY (Default value)"
			echo "*****************************************************************************"
			;;
                3)      TEST_NUMBER=$1
                        CHANNELS=$2
                        BUFFER_SIZE=$3
                        REPOSITORY=$REPOSITORY_DEFAULT
			echo "*****************************************************************************"
                        echo "Test Number: $TEST_NUMBER"
                        echo "Channels: $CHANNELS"
                        echo "Buffer Size: $BUFFER_SIZE"
                        echo "Repository: $REPOSITORY"
			echo "*****************************************************************************"
                        ;;
		4)	TEST_NUMBER=$1
                        CHANNELS=$2
                        BUFFER_SIZE=$3
			REPOSITORY=$4
			echo "*****************************************************************************"
                        echo "Test Number: $TEST_NUMBER"
                        echo "Channels: $CHANNELS"
                        echo "Buffer Size: $BUFFER_SIZE"
			echo "Repository: $REPOSITORY"
			echo "*****************************************************************************"
                        ;;
                *)      echo "Usage: dma.sh TEST_NUMBER CHANNELS BUFFER_SIZE REPOSITORY"
			exit $EXIT_FAILURE
                        ;;
	esac

	# Export parameters
	export TEST_NUMBER
	export CHANNELS
	export BUFFER_SIZE
	export REPOSITORY
}

# Insert/Remove previous modules
InsertRemovePreMods()
{
        OP=$1
        if [ "$OP" = "INSERT" ]; then
                for PRE_MOD in $PRE_REQ
                do
                        insmod $FORCE $MODS_DIR/$PRE_MOD.$EXT
                done
        elif [ "$OP" = "REMOVE" ]; then
                for PRE_MOD in $PRE_REQ
                do
                      rmmod $PRE_MOD
                done
        else
                echo "Operation invalid"
                exit $EXIT_FAILURE
        fi
}

# TestDMA
# Parameters:
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
	cp $PROCFILE$MOD_NAME $REPOSITORY/$MOD_NAME

        echo "Removing module"
        rmmod $MOD_NAME
}

Generic()
{
	MOD_NAME=$1
	shift
	FILE=$1
	shift
	NUM=$CHANNELS

	echo "=== Testing Module: $MOD_NAME - Channels: $CHANNELS ==="
	TestDMA $NUM $MOD_NAME channels=$CHANNELS buf_size=$BUFFER_SIZE $*
	rm -f $FILE
}

GenericEndian()
{
        MOD_NAME=$1
        shift
        FILE=$1
        shift
        NUM=$CHANNELS

        echo -e "\n=== Testing Module: $MOD_NAME - Channels: $CHANNELS ==="
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
		1)	# Unchained
			HeadLine "Unchained channel Transfer"
			Generic dmaunlnk $TMP_FILE1
			rm $TMPBASE/results_files
			echo "$TMPBASE/dmaunlnk" >> $TMPBASE/results_files
			;;
		2)	# Static Chained
			HeadLine "Static chained Transfer"
			Generic dmastatic_chain $TMP_FILE1
                        rm $TMPBASE/results_files
                        echo "$TMPBASE/dmastatic_chain" >> $TMPBASE/results_files
			;;
		3) 	# Dynamic Chained
			HeadLine "Dynamic Chained Transfer"
			Generic dmadynamic_chain $TMP_FILE1
                        rm $TMPBASE/results_files
                        echo "$TMPBASE/dmadynamic_chain" >> $TMPBASE/results_files
			;;
		4) 	# Dynamic  Chained + unchained
			HeadLine "Unchained Channel & Dynamic Chained Transfer"
			touch $TMP_FILE1
			touch $TMP_FILE2
			Generic dmaunlnk $TMP_FILE1 &
			Generic dmadynamic_chain $TMP_FILE2 
                        rm $TMPBASE/results_files
                        echo "$TMPBASE/dmaunlnk" >> $TMPBASE/results_files
                        echo "$TMPBASE/dmadynamic_chain" >> $TMPBASE/results_files
			Waitup 4
			;;
		5) 	# Static Chained + unchained
			HeadLine "Unchained Channel & Static Chained Transfer"
			touch $TMP_FILE1
			touch $TMP_FILE2
			Generic dmaunlnk $TMP_FILE1 &
			Generic dmastatic_chain $TMP_FILE2 
                        rm $TMPBASE/results_files
                        echo "$TMPBASE/dmaunlnk" >> $TMPBASE/results_files
                        echo "$TMPBASE/dmastatic_chain" >> $TMPBASE/results_files
			Waitup 5
			;;
		6)	# Dynamic + Static Chained
			HeadLine "Dynamic & Static Chained Transfer"
			touch $TMP_FILE1
			touch $TMP_FILE2
			Generic dmadynamic_chain $TMP_FILE1 &
			Generic dmastatic_chain $TMP_FILE2 
                        rm $TMPBASE/results_files
                        echo "$TMPBASE/dmadynamic_chain" >> $TMPBASE/results_files
                        echo "$TMPBASE/dmastatic_chain" >> $TMPBASE/results_files
			Waitup 6
			;;
		7)	# Dynmic + Static Chained + unchained
			HeadLine "Unchained + Dynamic + Static Chained Transfer"
			touch $TMP_FILE1
			touch $TMP_FILE2
			touch $TMP_FILE3
			Generic dmadynamic_chain $TMP_FILE1 &
			Generic dmaunlnk $TMP_FILE3 &
			Generic dmastatic_chain $TMP_FILE2 
                        rm $TMPBASE/results_files
                        echo "$TMPBASE/dmadynamic_chain" >> $TMPBASE/results_files
                        echo "$TMPBASE/dmaunlnk" >> $TMPBASE/results_files
                        echo "$TMPBASE/dmastatic_chain" >> $TMPBASE/results_files
			Waitup 7
			;;
		8)	# UnChained priority enable
			HeadLine "Priority Test"
			Generic dmaunlnk $TMP_FILE1 prio=2
                        rm $TMPBASE/results_files
                        echo "$TMPBASE/dmaunlnk" >> $TMPBASE/results_files
			;;
		9)	# UnChained qmery index
			HeadLine "Query Test"
			Generic dmaunlnk $TMP_FILE1 query_idx=1
                        rm $TMPBASE/results_files
                        echo "$TMPBASE/dmaunlnk" >> $TMPBASE/results_files
			;;
		10)	# UnChained burst mode
			HeadLine "Enable burst mode in unchained"
			Generic dmaunlnk $TMP_FILE1 burst=1
                        rm $TMPBASE/results_files
                        echo "$TMPBASE/dmaunlnk" >> $TMPBASE/results_files
			;;
		11) 	# Static Chained
			HeadLine "Static chained + burst enable Transfer "
			Generic dmastatic_chain $TMP_FILE1 burst=1
                        rm $TMPBASE/results_files
                        echo "$TMPBASE/dmastatic_chain" >> $TMPBASE/results_files
			;;
		12)	# Dynamic Chained
			HeadLine "Dynamic Chained Transfer + burst enable"
			Generic dmadynamic_chain $TMP_FILE1 burst=1
                        rm $TMPBASE/results_files
                        echo "$TMPBASE/dmadynamic_chain" >> $TMPBASE/results_files
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

# Insert previous modules
InsertRemovePreMods "INSERT"

# Select and run test
SelectRunTest

# Clean-up temporal files
rm -f $TMP_FILE2 $TMP_FILE1 $TMP_FILE3

# Remove previous modules
InsertRemovePreMods "REMOVE"
