#!/bin/sh

OPTION=$1

ovl0=/sys/devices/platform/omapdss/overlay0
ovl1=/sys/devices/platform/omapdss/overlay1
ovl2=/sys/devices/platform/omapdss/overlay2

mgr0=/sys/devices/platform/omapdss/manager0
mgr1=/sys/devices/platform/omapdss/manager1

lcd=/sys/devices/platform/omapdss/display0
tv=/sys/devices/platform/omapdss/display1


fb0=/sys/class/graphics/fb0
fb1=/sys/class/graphics/fb1

if [ $OPTION -eq 1 ] ; then

echo "UI is on video Overlay 1 on the TV "
echo 0 > $ovl0/enabled
echo 0 > $ovl1/enabled
echo " 0, 1" > $fb0/overlays
echo "tv" > $ovl1/manager
echo "0,0" > $ovl1/position
echo "720,574" > $ovl1/output_size
echo 1 > $ovl0/enabled
echo 1 > $ovl1/enabled
echo 1 > $tv/enabled
sleep 5
echo "Configuring the video overlay1 to default configuration"

echo "0" > $ovl0/enabled
echo "0" > $ovl1/enabled
echo " 0" > $fb0/overlays
echo "lcd" > $ovl1/manager
echo "0,0" > $ovl1/position
echo "800,480" > $ovl1/output_size
echo "1" > $ovl0/enabled
echo "0" > $tv/enabled
exit 0

elif [ $OPTION -eq 2 ] ; then
echo "UI is on video Overlay 2 on the TV "
echo "0" > $ovl0/enabled
echo "0" > $ovl2/enabled
echo " 0, 2" > $fb0/overlays
echo "tv" > $ovl2/manager
echo "0,0" > $ovl2/position
echo "720,574" > $ovl2/output_size
echo "1" > $ovl0/enabled
echo "1" > $ovl2/enabled
echo "1" > $tv/enabled

sleep 5
echo "Configuring the video overlay2 to default configuration"

echo "0" > $ovl0/enabled
echo "0" > $ovl2/enabled
echo "0" > $fb0/overlays
echo "lcd" > $ovl2/manager
echo "0,0" > $ovl2/position
echo "800,480" > $ovl2/output_size
echo "1" > $ovl0/enabled
echo "0" > $tv/enabled
exit 0
fi
