CC := $(CROSS_COMPILE)gcc
CPP := $(CROSS_COMPILE)cpp 

SUBDIRS := bonnie_app mtd_app misc_app fs_app preemption_app

all: $(SUBDIRS)

install:
	make -C Bonnie install
	make -C mtd-utils install
	make -C misc install
	make -C fs install
	make -C preemption install

bonnie_app:
	make -C Bonnie

mtd_app:
	make -C mtd-utils

misc_app: 
	make -C misc

fs_app:
	make -C fs

preemption_app:
	make -C preemption
clean:
	make -C Bonnie clean
	make -C mtd-utils clean
	make -C misc clean
	make -C fs clean
	make -C preemption clean
