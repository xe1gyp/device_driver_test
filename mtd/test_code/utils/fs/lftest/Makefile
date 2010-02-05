CC=$(CROSS_COMPILE)gcc
CFLAGS+= -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE
SRCS=$(wildcard *.c)
TARGETS=$(patsubst %.c,%,$(SRCS))

all: $(TARGETS)

install:
	@set -e; for i in $(TARGETS); do ln -f $$i ../../../binaries/$$i ; done

clean:
	rm -f $(TARGETS)
