CC := $(CROSS_COMPILE)gcc

TARGETS := Bonnie
FLAGS = -O $(SYSFLAGS)

all: $(TARGETS)

bsd:
	@echo 'Options are "make bsd" and "make SysV" - the default is "bsd".'
	@echo 'If you get messages about missing functions, try "make SysV."'
	make Bonnie

SysV:
	make Bonnie 'SYSFLAGS=-DSysV'

install:
	cp Bonnie $(SUITE_DIR)/bin

$(TARGETS): $(TARGETS)*.c

clean:
	rm -vf Bonnie
