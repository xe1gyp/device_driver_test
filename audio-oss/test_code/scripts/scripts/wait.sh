#!/bin/bash
if [ $# -lt 1 ]; then
	echo "usage: appname [sleep duration] $#"
	exit 1
fi
app=$1
if [ $# -eq 2 ]; then
	sl=$2
else
	sl=10
fi
function eq
{
K=`ps -ef|grep "$app"|grep -v bash|grep -v grep|grep -v wait.sh`
}

eq
while [ x"$K" != x ]
do
	echo "Waiting for $app $sl -$K"
	sleep 1;
	if [ $sl -le 0 ]; then
		echo "Timedout"
		break
	fi
	sl=`expr $sl - 1`
	eq
done
exit 0
