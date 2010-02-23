###########################################################################
# Makefile for the device driver test repository.
# (C) Copyright Texas Instruments, 2010. All Rights Reserved.
#
# NOTE: The following variables must be exported for a successfull
#	compilation. Default values are provided in the makefile for
#	variables that have not been explicitly set.
#	except for KDIR, which should be specified on new builds).
#
#	KDIR=<path to kernel source>
#	ARCH=<architecture of the platform>
#	CROSS_COMPILE=<toolchain prefix>
#	HOST=<name of the host machine>
#	TESTSUITES=<set of testsuites to be compiled>
#
#	Author: Aharon Campos <aharon.campos@ti.com>
#
############################################################################

all: maketest

check:
-include .config
-include $(KDIR)/.config

TESTROOT 	:= build
CODE_DIR	:= test_code
UTILSDIR 	:= $(PWD)/utils
SCENARIOS	:= scripts/scenarios

OLD_KDIR:=$(shell cat .config | grep KDIR | cut -d'=' -f2)
OLD_SUITES:=$(shell cat .config | grep TESTSUITES | cut -d'=' -f2)

define scenario_list
	@cat $@/$(CODE_DIR)/$(SCENARIOS)/applicability |\
	grep $(TARGET_PLATFORM) | grep $(TARGET_FILESYSTEM) |\
	cut -d' ' -f1 > /tmp/applicable.scenarios
	@echo "$(TESTROOT)/$@/$(SCENARIOS): \
	`cat /tmp/applicable.scenarios |\
	sed -e "s/^/$@\/$(CODE_DIR)\/scripts\/scenarios\//g" |\
	tr '\n' ' '`" >> dependencies
	@rm /tmp/applicable.scenarios
endef

ifdef CONFIG_ARCH_OMAP4
TARGET_PLATFORM:=OMAP_4430
endif
ifdef CONFIG_MACH_OMAP_3430SDP
TARGET_PLATFORM:=OMAP_3430
endif
ifdef CONFIG_MACH_OMAP_ZOOM2
TARGET_PLATFORM:=OMAP_3430
endif
ifdef CONFIG_MACH_OMAP_3630SDP
TARGET_PLATFORM:=OMAP_3630
endif
ifdef CONFIG_MACH_OMAP_ZOOM3
TARGET_PLATFORM:=OMAP_3630
endif
ifdef CONFIG_ANDROID
TARGET_FILESYSTEM:=ANDROID
else
TARGET_FILESYSTEM:=LINUX_GENERIC
endif

ifeq ($(KDIR),)
$(error KDIR must be defined to proceed)
endif
ifeq ($(TARGET_PLATFORM),)
$(error The kernel directory has not been properly configured)
endif
ifeq ($(ARCH),)
$(warning ARCH was not specified. Defaulting to "arm")
ARCH:=arm
endif
ifeq ($(HOST),)
$(warning HOST was not specified. Defaulting to "...")
HOST:=...
endif
ifeq ($(CROSS_COMPILE),)
$(warning CROSS_COMPILE was not specified. \
Defaulting to "arm-none-linux-gnueabi-")
CROSS_COMPILE:=arm-none-linux-gnueabi-
endif
ifeq ($(TESTSUITES),)
$(warning TESTSUITES was not specified. Defaulting to "all")
TESTSUITES:=all
endif

ifeq ($(TESTSUITES),all)
 ifeq ($(TARGET_PLATFORM),OMAP_4430)
  override TESTSUITES:= audio-alsa dma framebuffer gpio hsuart i2c \
  mcbsp mcspi mmc nand norflash performance \
  realtimeclock timer-32k video watchdog
 else
  ifeq ($(TARGET_FILESYSTEM),ANDROID)
   override TESTSUITES:= audio-alsa camera dma ethernet framebuffer \
   gpio hsuart i2c keypad mcbsp mmc nand norflash \
   power_management realtimeclock timer-32k \
   touchscreen usb_device usb_ehci usb_host
  else
   override TESTSUITES:= camera ethernet framebuffer \
   hdq mcspi nand neon \
   realtimeclock security \
   touchscreen
  endif
 endif
endif

export KDIR
export ARCH
export CROSS_COMPILE
export HOST
export TARGET_PLATFORM
export TARGET_FILESYSTEM
export TESTSUITES
export UTILSDIR

.PHONY: all check cleanup maketest clean distclean utilities $(TESTSUITES)

cleanup: check
	@if [ $(KDIR) = $(OLD_KDIR) ]; then \
		echo "Removing unwanted testsuites \
		\"$(filter-out $(TESTSUITES), $(OLD_SUITES))\"..."; \
		cd $(TESTROOT); \
		rm -rf $(filter-out $(TESTSUITES), $(OLD_SUITES)); \
	else 	if [ -d $(TESTROOT) ]; then \
			echo "KDIR has changed. Cleaning output directory..."; \
			sleep 2; \
			$(MAKE) clean; \
		fi; \
	fi;

maketest: cleanup
	@-cat .config > .oldconfig
	@echo "KDIR=$(KDIR)" > .config
	@echo "ARCH=$(ARCH)" >> .config
	@echo "CROSS_COMPILE=$(CROSS_COMPILE)" >> .config
	@echo "HOST=$(HOST)" >> .config
	@echo "TESTSUITES=$(TESTSUITES)" >> .config
	@echo "Building for platform $(TARGET_PLATFORM)" \
	"and filesystem $(TARGET_FILESYSTEM)... "
	@sleep 2
	@$(MAKE) $(TESTSUITES)
	@$(MAKE) $(addprefix $(TESTROOT)/,$(TESTSUITES))
	@$(MAKE) $(addsuffix /$(SCENARIOS),$(addprefix $(TESTROOT)/,$(TESTSUITES)))
	@$(MAKE) utilities
	@$(MAKE) $(TESTROOT)/$(UTILSDIR)

$(TESTSUITES):
	@echo
	@echo "======== Compiling $(subst /$(CODE_DIR),,$@) testsuite ========"
	@echo
	@sleep 1
	@mkdir -p $(TESTROOT)
	@$(MAKE) -C $@/$(CODE_DIR) SUITE_DIR=$(PWD)/$@/$(CODE_DIR)
	$(call scenario_list)
-include dependencies

.SECONDEXPANSION:
$(addprefix $(TESTROOT)/,$(TESTSUITES)): $$(notdir $$@)/$(CODE_DIR)
	@mkdir -p $@
	@cd $(subst $(TESTROOT)/,,$@)/$(CODE_DIR) && \
	find . -not -name \*.[ao] -not -type d | xargs file | \
	grep -vi ascii | grep -vi shell | cut -d: -f1 | \
	xargs -I '{}' cp --parents '{}' $(PWD)/$@
	@echo
	@echo "====Installed \"$(notdir $@)\" testsuite in \"$(TESTROOT)\"====";
	@echo

$(addsuffix /$(SCENARIOS),$(addprefix $(TESTROOT)/,$(TESTSUITES))):
	@mkdir --parents $@
	@cp $? $@
	@echo
	@echo "====Installed scenario files in $@====";
	@echo

utilities:
	@$(MAKE) -C $(UTILSDIR)

$(TESTROOT)/$(UTILSDIR): $(UTILSDIR)
	@find $(notdir $(UTILSDIR)) ! -name \*.[ao] ! -type d | \
	xargs file | grep -vi ascii | cut -d: -f1 | \
	xargs -I '{}' cp --parents '{}' $(TESTROOT)
	@echo
	@echo "=========Installed utils directory in \"$(TESTROOT)\"=========";
	@echo

clean:
	@for dir in $(shell ls -d */); do \
	if [ -f $${dir}$(CODE_DIR)/Makefile ]; then \
		$(MAKE) -C $${dir}$(CODE_DIR) \
		SUITE_DIR=$(PWD)/$${dir}$(CODE_DIR) clean; \
	fi; \
	done
	@$(MAKE) -C $(UTILSDIR) clean
	-rm -rf build
	-rm dependencies

distclean: clean
	-rm .config .oldconfig
