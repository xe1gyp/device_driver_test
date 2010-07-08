#!/bin/sh -x

if [ "$1" "$2" "$3" ]; then
	exit 0
else
	exit 1
fi
