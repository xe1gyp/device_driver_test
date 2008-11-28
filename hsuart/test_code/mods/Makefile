# Comment/uncomment the following line to disable/enable debugging
# DEBUG = y

export CC:=$(CROSS_COMPILE)gcc
export KERN:=26

ifneq ($(KERNELRELEASE),)
ifeq ($(KERN),26)
    obj-m       := hsuart.o
else
    export-objs     :=  hsuart.o
    obj-m +=hsuart.o
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

clean:
	$(RM) -r *.o *.ko *.mod.c .*.cmd .tmp_versions .*.flags .26

