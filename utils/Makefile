SUBDIRS:=akey pan priority schedutils evtest microcom

ifeq ($(UTILSDIR),)
UTILSDIR=$(PWD)
endif

export CC := $(CROSS_COMPILE)gcc
export UTILS_BINARIES = $(UTILSDIR)/bin

.PHONY: $(SUBDIRS) install clean

all: $(SUBDIRS) install

$(SUBDIRS):
	$(MAKE) -C $@

install:
	mkdir -p $(UTILS_BINARIES)
	$(foreach folder,$(SUBDIRS),$(MAKE) -C $(folder) install;)

clean:
	$(foreach folder,$(SUBDIRS),$(MAKE) -C $(folder) clean;)
	rm -rf $(UTILS_BINARIES)
