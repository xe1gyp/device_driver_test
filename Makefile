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
.EXPORT_ALL_VARIABLES:

all: maketest

check:
-include .config
-include $(KDIR)/.config

TESTROOT 	:= $(PWD)/build
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
CFLAGS+=-static
LDFLAGS+=-static
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
$(warning HOST was not specified. Defaulting to "x86")
HOST:=x86
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

APPLICABLE_TESTS :=     benchmarks dma ethernet gpio hsuart i2c \
                        keypad mcbsp mcspi mmc oskernel performance \
                        realtimeclock timer-32k watchdog
ifdef CONFIG_ARCH_OMAP3
APPLICABLE_TESTS +=     camera, audio, framebuffer, video
endif
ifdef CONFIG_ANDROID
APPLICABLE_TESTS +=     android
endif
ifneq ($(or $(CONFIG_MACH_OMAP_3630SDP), $(CONFIG_MACH_OMAP_3430SDP)),)
APPLICABLE_TESTS +=     norflash
endif

ifeq ($(TESTSUITES),all)
override TESTSUITES := $(APPLICABLE_TESTS)
endif

CFLAGS+= -D $(TARGET_PLATFORM) -D $(TARGET_FILESYSTEM)

.PHONY: all check cleanup maketest clean distclean utilities $(TESTSUITES)\
$(TESTROOT)/$(notdir $(UTILSDIR)) $(addprefix $(TESTROOT)/,$(TESTSUITES))

cleanup: check
	@if [ -d $(TESTROOT) ]; then\
		if [ $(KDIR) = $(OLD_KDIR) ]; then \
			echo "Removing unwanted testsuites \
			\"$(filter-out $(TESTSUITES), $(OLD_SUITES))\"..."; \
			cd $(TESTROOT); \
			rm -rf $(filter-out $(TESTSUITES), $(OLD_SUITES)); \
		else \
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
	@$(MAKE) $(TESTROOT)/$(notdir $(UTILSDIR))

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
	@-cp -r $</bin $@
	@-cp -r $</scripts $@
	@-cp -r $</modules $@
	@mkdir -p $@/scripts/tmp
	@mkdir -p $@/scripts/test
	@rm -rf $@/$(SCENARIOS)
	@echo
	@echo "====Installed \"$(notdir $@)\" testsuite in \"$(TESTROOT)\"====";
	@echo

$(addsuffix /$(SCENARIOS),$(addprefix $(TESTROOT)/,$(TESTSUITES))):
	@if [ '$?' != '' ]; then \
		mkdir --parents $@; \
		cp $? $@; \
		echo; \
		echo "====Installed scenario files in $@===="; \
		echo; \
	else \
		echo "======================================="\
		"======================================="; \
		echo "WARNING: No scenario files are being copied"\
		"to $@!"; \
		echo "======================================="\
		"======================================="; \
		sleep 3; \
	fi;

utilities:
	@$(MAKE) -C $(UTILSDIR)

$(TESTROOT)/$(notdir $(UTILSDIR)): $(UTILSDIR)
	@mkdir -p $@
	@cp -r $</scripts $@
	@cp -r $</configuration $@
	@cp -r $</bin $@
	@cp -r $</handlers $@
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
