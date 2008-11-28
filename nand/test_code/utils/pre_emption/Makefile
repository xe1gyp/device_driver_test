CC := $(CROSS_COMPILE)gcc

TARGETS := proc1 proc2
FLAGS = -O $(SYSFLAGS)

all: $(TARGETS)

bsd:
	@echo 'Options are "make bsd" and "make SysV" - the default is "bsd".'
	@echo 'If you get messages about missing functions, try "make SysV."'
	make Bonnie

SysV:
	make pre_emption 'SYSFLAGS=-DSysV'

install:
	cp -fv $(TARGETS) ../../bin

$(TARGETS):
	$(CC) $@.c -o $@

clean:
	rm -vf pre_emption
	rm -fv $(TARGETS)
