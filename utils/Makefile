ifneq ($(strip $(TESTSUITES_ROOT)),)
 export UTILS_BINARIES := ${TESTSUITES_ROOT}/utils/bin
else
 export UTILS_BINARIES := ${PWD}/bin
endif

all: pan_app akey_app priority_app schedutils_app install

install:
	mkdir -p $(UTILS_BINARIES)
	make -C akey install
	make -C pan install
	make -C priority install
	make -C schedutils install

pan_app:
	make -C pan
	
akey_app:
	make -C akey

priority_app:
	make -C priority

schedutils_app:
	make -C schedutils

clean:
	make -C pan clean
	make -C akey clean
	make -C priority clean
	make -C schedutils clean
	rm -rf $(UTILS_BINARIES)
