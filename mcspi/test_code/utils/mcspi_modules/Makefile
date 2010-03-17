CC:=$(CROSS_COMPILE)gcc
KERN:=26

ifneq ($(KERNELRELEASE),)

	ifeq ($(KERN),26)
		obj-m := omap_mcspi_datatest.o
	else
		export-objs :=  omap_mcspi_datatest.o
		obj-m += omap_mcspi_datatest.o
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
	cp *.ko ../../modules/

clean:
	$(RM) -r *.o *.ko *.mod.c .o.* .*.cmd .tmp_versions
	$(RM) -r .*.flags .26 Module.symvers modules.order
