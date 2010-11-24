#!/bin/sh

PIX_FORMAT=$1
RESULT=0

#Usage: ./tv_settiming.sh <pix_format>
#pix_format 1 : set interlace format, timing value = 51
#pix_format 0 : set progressive format, timing value = 161

if [ "$PIX_FORMAT" = "1" ];then
	echo 51 > /sys/devices/platform/omapdss/display2/custom_edid_timing
elif [ "$PIX_FORMAT" = "1" ];then
	echo 161 > /sys/devices/platform/omapdss/display2/custom_edid_timing
fi
sleep 1
exit $RESULT
