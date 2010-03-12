CC := $(CROSS_COMPILE)gcc

TARGETS := proc1 proc2
FLAGS = -O $(SYSFLAGS)

all: $(TARGETS)

bsd:
	@echo 'Options are "make bsd" and "make SysV" - the default is "bsd".'
	@echo 'If you get messages about missing functions, try "make SysV."'
	make Bonnie

SysV:
	make preemption 'SYSFLAGS=-DSysV'

install:
	cp -fv $(TARGETS) $(SUITE_DIR)/bin

$(TARGETS):
	$(CC) $@.c -o $@

clean:
	rm -vf preemption
	rm -fv $(TARGETS)
