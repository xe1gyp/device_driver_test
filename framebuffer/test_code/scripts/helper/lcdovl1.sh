echo "1" > /sys/devices/platform/omapdss/display0/enabled
sleep 1
echo "0" > /sys/devices/platform/omapdss/overlay1/enabled
echo "lcd" > /sys/devices/platform/omapdss/overlay1/manager
echo "1" > /sys/devices/platform/omapdss/overlay1/enabled
echo "0" > /sys/devices/platform/omapdss/overlay1/enabled
echo "lcd" > /sys/devices/platform/omapdss/overlay1/manager
echo "1" > /sys/devices/platform/omapdss/overlay1/enabled

sleep 2

