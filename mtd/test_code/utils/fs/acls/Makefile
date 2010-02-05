CC=$(CROSS_COMPILE)gcc

check_header = $(shell \
	if echo "\#include <$(1)>" | $(CC) -E - > /dev/null 2>&1 ; \
	then echo yes ; \
	else echo no ; fi)

ifeq ($(call check_header,sys/xattr.h),yes)

all: acl_file_test acl_link_test

acl_file_test:
	$(CC) -o acl_file_test acl_file_test.c
acl_link_test:
	$(CC) -o acl_link_test acl_link_test.c

install:
	ln -f acl_file_test ../../../binaries/
	ln -f acl_link_test ../../../binaries/
	ln -f acl_test01 ../../../binaries/

else

all install:
	@echo File sys/xattr.h does not exist, attr tests cannot be build!
endif

clean:
	rm -f acl_file_test acl_link_test
