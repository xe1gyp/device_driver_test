#/* ==========================================================================
#             Texas Instruments OMAP(TM) Platform Software
#  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
#
#  Use of this software is controlled by the terms and conditions found
#  in the license agreement under which this software has been supplied.
# ========================================================================== */

SUBDIRS = gpio_mods_dir

all: $(SUBDIRS)

install:
	$(MAKE) -C gpio_mods install

gpio_apps_dir:
	$(MAKE) -C gpio_apps

gpio_mods_dir:
	$(MAKE) -C gpio_mods

clean:
	$(MAKE) -C gpio_mods clean
