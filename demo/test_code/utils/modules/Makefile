CC:=$(CROSS_COMPILE)gcc
KERN:=26

ifneq ($(KERNELRELEASE),)

	ifeq ($(KERN),26)
		obj-m := hello.world.o
	else
		export-objs :=  hello.world.o
		obj-m += hello.world.o
		include $(TOPDIR)/Rules.make
	endif
else
	PWD=$(shell pwd)
	ifeq ($(KERN),26)
		a :=$(shell touch .26)
	endif

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) KERN=$(KERN) modules
endif

install:
	mv *.ko ../../modules/

clean:
	$(RM) -r *.o *.o.* *.ko *.mod.c .*.cmd .tmp_versions
	$(RM) -r .*.flags .26 Module.symvers modules.order
