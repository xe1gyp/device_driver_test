ifeq ($(SUITE_DIR),)
SUITE_DIR:=$(PWD)
endif

CC:=$(CROSS_COMPILE)gcc
TARGETS := spidev_test spidev_fdx

all: $(TARGETS)

$(TARGETS):
	$(CC) $(CFLAGS) $@.c -o $@

install:
	cp -vf $(TARGETS) $(SUITE_DIR)/bin

clean:
	rm -vf $(TARGETS)
