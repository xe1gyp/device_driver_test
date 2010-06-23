#!/bin/sh

# =============================================================================
# Variables
# =============================================================================

LOCAL_COMMAND=$1
LOCAL_SIZE=$2
LOCAL_LOOPS=$3

# =============================================================================
# Functions
# =============================================================================

# None

memTesterFieldsPopulate() {

touch $HMT_MEMTESTER_FIELDS_FILE

cat <<-EOF > $HMT_MEMTESTER_FIELDS_FILE
Stuck Address
Random Value
Compare XOR
Compare SUB
Compare MUL
Compare DIV
Compare OR
Compare AND
Sequential Increment
Solid Bits
Block Sequential
Checkerboard
Bit Spread
Bit Flip
Walking Ones
Walking Zeroes
EOF

}

# =============================================================================
# Main
# =============================================================================

handlerError.sh "test"
if [ $? -eq 1 ]; then
	exit 1
fi

if [ "$LOCAL_COMMAND" = "run" ]; then

	memtester $LOCAL_SIZE $LOCAL_LOOPS > $HMT_MEMTESTER_LOG_FILE
	memTesterFieldsPopulate

	LOCAL_INSTANCE=0
	while read LOCAL_FIELD
	do
		LOCAL_INSTANCE=`expr $LOCAL_INSTANCE + 1`
		echo "$LOCAL_FIELD"
		cat $HMT_MEMTESTER_LOG_FILE | grep "$LOCAL_FIELD" | grep ok
		if [ $? != 0 ]; then
			echo "Fatal: handlerMemTester.sh: $LOCAL_FIELD failed!"
			exit 1
		fi

	done < $HMT_MEMTESTER_FIELDS_FILE

fi

# End of file
