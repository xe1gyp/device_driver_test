CC := $(CROSS_COMPILE)gcc
CFLAGS += -g -O -Wall -I$(KDIR)

TARGETS = evtest testkeypad dev_name

all: $(TARGETS)

install:
	cp -vf $(TARGETS) ../../binaries

$(TARGETS):
	$(CC) $(CFLAGS) $@.c -o $@

clean:
	rm -vf $(TARGETS)
