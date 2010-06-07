#!/bin/sh

get_video_res()
{
	#vrp=`${PWD}/../helper/fbout | grep Geometry | sed -e 's/[a-zA-Z-]*//g' \
	#	| sed -e 's/,//g' | awk '{ x=$1; y=$2; print x " " y; }'`
	$TESTBIN/setimg $VIDEO_PIPELINE $SETIMG_PARAMETERS > $TMPBASE/setimgtmp.txt
	RESULT=`command_tracking.sh $RESULT $?`
	vrp=`cat $TMPBASE/setimgtmp.txt | grep window | sed -e 's/[a-zA-Z-]*//g' \
							| sed -e 's/=//g'`
	cat $TMPBASE/setimgtmp.txt
	set $vrp
	WIDTH_MAX=$3
	HEIGHT_MAX=$4
	rm $TMPBASE/setimgtmp.txt
	img_size
	return
}

img_size()
{
	WIDTH="$WIDTH_MAX"
	WIDTH=$(($WIDTH / $SCALE_FACTOR))
	HEIGHT="$HEIGHT_MAX"
	HEIGHT=$(($HEIGHT / $SCALE_FACTOR))
}

left_top_corn()
{
	LEFT=0
	TOP=0
}

rigth_top_corn()
{
	LEFT="$WIDTH_MAX"
	LEFT=$(($LEFT - $LEFT / 3))
	TOP=0
}

rigth_bottom_corn()
{
	LEFT="$WIDTH_MAX"
	LEFT=$(($LEFT - $LEFT / 3))
	TOP="$HEIGHT_MAX"
	TOP=$(($TOP - $TOP / 3))
}

left_bottom_corn()
{
	LEFT=0
	TOP="$HEIGHT_MAX"
	TOP=$(($TOP - $TOP / 3))
}

centered()
{
	LEFT="$WIDTH_MAX"
	LEFT=$(($LEFT / 3))
	TOP="$HEIGHT_MAX"
	TOP=$(($TOP / 3))
	WIDTH="$WIDTH_MAX"
	WIDTH=$(($WIDTH - $LEFT \* 2))
	HEIGHT="$HEIGHT_MAX"
	HEIGHT=$(($HEIGHT - $TOP \* 2))
	return
}

stream()
{
	#Usage: setcrop <vid> <left> <top> <width> <height>
	if [ ! -z "$SETCROP_PARAMETERS" ]; then
		$TESTBIN/setcrop $VIDEO_PIPELINE $SETCROP_PARAMETERS
		RESULT=`command_tracking.sh $RESULT $?`
	fi

	#Usage: setwin <vid> <left> <top> <width> <height>
	$TESTBIN/setwin $VIDEO_PIPELINE $LEFT $TOP $WIDTH $HEIGHT
	RESULT=`command_tracking.sh $RESULT $?`

	#Usage: streaming <vid> <inputfile> [<n>]
	if [ $IS_OMAP4 eq 1 ]
	then
		$TESTBIN/streaming_tiler $VIDEO_PIPELINE $STREAMING_PARAMETERS 0
	else
		$TESTBIN/streaming $VIDEO_PIPELINE $STREAMING_PARAMETERS 0
	fi
	RESULT=`command_tracking.sh $RESULT $?`
}

left_to_right_top()
{
	left_top_corn
	stream
	DeltaX=$(($WIDTH))
	LEFT=$(($LEFT + $DeltaX))
	stream
}

right_to_left_bottom()
{
	rigth_bottom_corn
	stream
	DeltaX=$(($WIDTH))
	LEFT=$(($LEFT - $DeltaX))
	stream
}

top_to_bottom_right()
{
	rigth_top_corn
	stream
	DeltaY=$(($HEIGHT))
	TOP=$(($TOP + $DeltaY))
	stream
}

bottom_to_top_left()
{
	left_bottom_corn
	stream
	DeltaY=$(($HEIGHT))
	TOP=$(($TOP - $DeltaY))
	stream
}

all_windows()
{
	left_to_right_top
	top_to_bottom_right
	right_to_left_bottom
	bottom_to_top_left

	centered
	stream
	LEFT=0
	TOP=0
	WIDTH=$(($WIDTH_MAX))
	HEIGHT="$HEIGHT_MAX"
	stream
}


main()
{
	VIDEO_PIPELINE=$1
	SETIMG_PARAMETERS=$2
	SETWIN_OPTION=$3
	STREAMING_PARAMETERS=$4
	SETCROP_PARAMETERS=$5
	RESULT=0
	SCALE_FACTOR=3

	cat /sys/devices/platform/omapdss/manager0/display | grep hdmi

	if [ $? -eq 0 ]
	then
		echo "HDMI is enabled, maximum supported downscaling is 0.5x"
		SCALE_FACTOR=2
	fi

	#if [ -z $1 ]
	#then
	#	parm="h"
	#if [ -n $1 ]
	#then
	#	parm=$1
	#fi
	#case $parm in
	get_video_res
	case $SETWIN_OPTION in
	1)
		left_top_corn
		stream ;;
	2)
		rigth_top_corn
		stream ;;
	3)
		rigth_bottom_corn
		stream ;;
	4)
		left_bottom_corn
		stream ;;
	5)
		centered
		stream ;;
	6)
		all_windows ;;
	\?) # Handle illegals
		;;
	esac


	if [ -z "$STRESS" ]; then
		stress_message.sh
	fi
	exit $RESULT
}

main "$@"
