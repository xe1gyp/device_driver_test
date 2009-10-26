export CC:=$(CROSS_COMPILE)gcc
export KERN:=26

MODULES:=cts_uart_wakeup.o

ifneq ($(KERNELRELEASE),)
ifeq ($(KERN),26)
    obj-m       := $(MODULES)
else
    export-objs     :=  $(MODULES)
    obj-m +=cts_uart_wakeup.o
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
	cp -v $(MODULES:.o=.ko) ../../mods
clean:
	$(RM) -r *.o *.ko *.mod.c .*.cmd .tmp_versions .*.flags .26 \
	Module.symvers modules.order .cts_uart_wakeup.o.d
