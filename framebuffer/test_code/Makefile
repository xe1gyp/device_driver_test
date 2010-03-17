export CC:=$(CROSS_COMPILE)gcc

all: applications install

install:
	mkdir -p bin
	mkdir -p scripts/tmp
	mkdir -p scripts/test
	make -C utils install

applications:
	make -C utils CC=$(CC)

clean:
	make -C utils clean
	rm -vfr bin/
	rm -vfr scripts/tmp
	rm -vfr scripts/test
	rm -f scripts/log.*
