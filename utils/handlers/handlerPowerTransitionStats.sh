#!/bin/sh

#
#  Power Transitions Statistics handler
#
#  Copyright (c) 2010 Texas Instruments
#
#  Author: Arce Abraham <x0066660@ti.com>
#  Author: Leed Aguilar <leed.aguilar@ti.com>
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License as
#  published by the Free Software Foundation; either version 2 of the
#  License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
#  USA
#

# =============================================================================
# Variables
# =============================================================================

operation=$1
error_val=0

# =============================================================================
# Functions
# =============================================================================

# Prints a message with a specific format
# @ Function: showInfo
# @ Parameters: <message to display>
# @ Return: None
showInfo() {
	message="$@"
	echo "[ handlerPowerTransitionStats ] $message"
}

# =============================================================================
# Main
# =============================================================================

handlerError.sh "test"
if [ $? -eq 1 ]; then
	exit 1
fi

handlerDebugFileSystem.sh "mount"

if [ "$operation" = "log" ]; then

	pwrdm=$2
	pwr_state=$3
	log_name=$4
	pwr_state_place=0

	if [ "$pwr_state" = "DEVICE-OFF" ]; then
		pwr_state_place=1
	elif [ "$pwr_state" = "OFF" ]; then
		pwr_state_place=2
	elif [ "$pwr_state" = "RET" ]; then
		pwr_state_place=3
	elif [ "$pwr_state" = "INA" ]; then
		pwr_state_place=4
	elif [ "$pwr_state" = "ON" ]; then
		pwr_state_place=5
	elif [ "$pwr_state" = "RET-LOGIC-OFF" ]; then
		pwr_state_place=6
	fi

	pwrdm_stat=`cat $PM_COUNT | grep ^$pwrdm | \
                cut -d "," -f $pwr_state_place`
	showInfo "Power domain stats requested: $pwrdm: $pwrdm_stat"
	pwrdm_value=`echo $pwrdm_stat | cut -d ":" -f 2`
	echo $pwrdm_value > $UTILS_DIR_TMP/pts.$pwrdm.$pwr_state.$log_name

elif [ "$operation" = "compare" ]; then

	pwrdm=$2
	pwr_state=$3
	log_name_before=$4
	log_name_after=$5
	val_before=`cat $UTILS_DIR_TMP/pts.$pwrdm.$pwr_state.$log_name_before`
	val_after=`cat $UTILS_DIR_TMP/pts.$pwrdm.$pwr_state.$log_name_after`

	showInfo "$pwrdm: Initial Value -> $pwr_state: $val_before"
	showInfo "$pwrdm: Final Value -> $pwr_state: $val_after"

	# Verify the power domain counter increases
	showInfo "Verifying $pwrdm: $pwr_state counter increases ..."
	sleep 3

	if [ $val_before -eq $val_after ]; then
		showInfo "ERROR: $pwrdm: $pwr_state counters did not increase"
		showInfo "TEST FAILED"
		error_val=1
	elif [ $val_after -gt $val_before ]; then
		showInfo "SUCCESS: $pwrdm: $pwr_state counters increased"
		showInfo "TEST PASSED"
		error_val=0
	else
		showInfo "FATAL: Please review power states counters"
		error_val=1
	fi
fi

handlerDebugFileSystem.sh "umount"

exit $error_val

# End of file
