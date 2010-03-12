CC=$(CROSS_COMPILE)gcc
CFLAGS += -w -O
LDFLAGS += 

SRCS=$(wildcard *.c)
TARGETS=$(patsubst %.c,%,$(SRCS))

all: $(TARGETS)

install:
	ln -f fsxtest02 $(SUITE_DIR)/bin
	ln -f fsxtest $(SUITE_DIR)/bin
	ln -f fsx-linux $(SUITE_DIR)/bin

clean:
	rm -f $(TARGETS)


