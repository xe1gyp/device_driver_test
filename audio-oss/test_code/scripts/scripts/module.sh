#!/bin/bash
export HOMEDIR=`dirname $0`
. $HOMEDIR/include.common
function help
{
	echo "Usage: $0 [insert/remove] [audio/buf/codec]"
	echo "Where insert will install the module"
	echo "      remove will remove the module"
	echo "      audio is the audio module"
	echo "      buf is the buffer management module"
	echo "      codec is the codec module"
}

if [ $# -lt 2 ]; then
	echo "Not enough arguments"
	help
	exit 1;
fi

CMD=$1
MOD=$2
if [ ! "$CMD" = "insert" -a ! "$CMD" = "remove" ]; then
	echo "CMD[$CMD] should be insert or remove"
	help
	exit 1
fi
if [ ! "$MOD" = "audio" -a ! "$MOD" = "buf"  -a ! "$MOD" = "codec" ]; then
	echo "MOD[$MOD] should be audio/buf/codec"
	help
	exit 1
fi
#MODDIR
case $MOD in
	audio)
		MODULE=$AUDIO_MODULE
		;;
	buf)
		MODULE=$BUF_MODULE
		;;
	codec)
		MODULE=$CODEC_MODULE
		;;
esac

case $CMD in
	insert)
		insmod $MODDIR/$MODULE
		RET=$?
		;;
	remove)
		MODULE=`echo "$MODULE"|cut -d '.' -f1`
		rmmod $MODULE
		RET=$?
		;;
esac

if [ $RET -eq 0 ]; then
	echo "Operation Success"
else
	echo "Operation Failure"
fi

exit $RET
