#!/bin/bash
# common info
export HOMEDIR=`dirname $0`/.
export EXEC_FILE=1
. $HOMEDIR/include.common
echo "STEREO=$STEREO BITS=$BITS FREQUENCY=$FREQUENCY"
if [ -f "$1" ]; then
	PLAYFILE_NAME=$1
fi
#-- Player
if [ -z "$STEREO" -o -z "$BITS" -o -z "$FREQUENCY" ]; then
	echo "bad params-check filename"
	exit 1
fi
if [ ! -f "$PLAYFILE_NAME" -o ! -r "$PLAYFILE_NAME" ]; then
	echo $PLAYFILE_NAME not found or invalid
	exit 2
fi
if [ "$STEREO" = "stereo" ]; then
	MONO_OPT="-m 2"
else
	MONO_OPT="-m 1"
fi
#play the file
$PLAYER -f $FREQUENCY -s $BITS -i $PLAYFILE_NAME $PLAYER_GENERIC_OPTION $MONO_OPT
