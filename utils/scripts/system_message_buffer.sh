#!/bin/sh

#
#  system_message_buffer.sh
#
#  Copyright (c) 2010 Texas Instruments
#
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

# This utility is useful to obtain information on the Kernel
# boot up process and system messages by using the 'dmesg'
# command or by examinating the /var/log/dmesg file.

# dmesg is used to examine or control the kernel ring buffer
# that upon boot, contains mainly the Kernel boot up information
# that is also available at /var/log/dmesg file.

# The main different between 'dmesg' and /var/log/dmesg is
# that 'dmesg' as command will always reflect what is happening
# in the Kernel ring buffer while /var/log/dmesg file is only
# a snapshot of all the system messages that happened before
# the Kernel logger takes the record of what is happening
#

# =============================================================================
# Local Variables
# =============================================================================

kernel_message_file=$1
error_val=0

# =============================================================================
# Functions
# =============================================================================

usage() {
	echo ""
	echo "---------------------------------------------"
	echo "usage: $0 <file name to save dmesg ouptut>"
	echo "---------------------------------------------"
	echo ""
	exit 1
}

# =============================================================================
# MAIN
# =============================================================================

if [ $# -ne 1 ]; then
	usage 1>&2
fi

if [ -f /var/log/dmesg ]; then
        alias dmesg='cat /var/log/dmesg'
fi

echo ""
echo "[ Testsuites ] System Messages are saved at: $kernel_message_file"
echo ""
dmesg > $kernel_message_file

exit $error_val

# End of file
