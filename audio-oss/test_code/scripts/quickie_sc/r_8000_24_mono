#!/bin/bash
# common info
export HOMEDIR=`dirname $0`/.
export EXEC_FILE=1
. $HOMEDIR/include.common
echo "STEREO=$STEREO BITS=$BITS FREQUENCY=$FREQUENCY"
#-- Player
if [ -z "$STEREO" -o -z "$BITS" -o -z "$FREQUENCY" ]; then
	echo "bad params-check filename"
	exit 1
fi
if [ "$STEREO" = "stereo" ]; then
	MONO_OPT="-m 2"
else
	MONO_OPT="-m 1"
fi
#play the file
$RECORDER -f $FREQUENCY -s $BITS $RECORDER_GENERIC_OPTION $MONO_OPT
