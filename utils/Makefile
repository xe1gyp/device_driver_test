CC := $(CROSS_COMPILE)gcc

export UTILS_BINARIES = $(UTILSDIR)/bin

#all: pan_app akey_app priority_app install
all: akey_app pan_app priority_app schedutils_app install

install:
	mkdir -p $(UTILS_BINARIES)
	make -C akey install
	make -C pan install
	make -C priority install
	make -C schedutils install

akey_app:
	make -C akey CC=${CC}

pan_app:
	make -C pan CC=${CC}

priority_app:
	make -C priority CC=${CC}

schedutils_app:
	make -C schedutils CC=${CC}

clean:
	make -C akey clean
	make -C pan clean
	make -C priority clean
	make -C schedutils clean
	rm -rf $(UTILS_BINARIES)
