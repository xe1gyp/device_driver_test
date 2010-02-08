echo "1" > /sys/devices/platform/omapdss/display0/enabled
sleep 1
echo "0" > /sys/devices/platform/omapdss/overlay0/enabled
echo "lcd" > /sys/devices/platform/omapdss/overlay0/manager
echo "1" > /sys/devices/platform/omapdss/overlay0/enabled
echo "0" > /sys/devices/platform/omapdss/overlay0/enabled
echo "lcd" > /sys/devices/platform/omapdss/overlay0/manager
echo "1" > /sys/devices/platform/omapdss/overlay0/enabled

sleep 2

