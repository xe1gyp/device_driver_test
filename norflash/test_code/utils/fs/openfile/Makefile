CC=$(CROSS_COMPILE)gcc
CFLAGS += -Wall
LOADLIBES += -lpthread

SRCS=$(wildcard *.c)
TARGETS=$(patsubst %.c,%,$(SRCS))

all: $(TARGETS)
 
install:
	@set -e; for i in $(TARGETS) ; do ln -f $$i $(SUITE_DIR)/bin/$$i ; done

clean:
	rm -f $(TARGETS)
