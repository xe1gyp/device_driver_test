# Comment/uncomment the following line to disable/enable debugging

export CC:=$(CROSS_COMPILE)gcc
export KERN:=26

ifneq ($(KERNELRELEASE),)
ifeq ($(KERN),26)
    obj-m       := cts_uart_wakeup.o
else
    export-objs     :=  cts_uart_wakeup.o
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

clean:
	$(RM) -r *.o *.ko *.mod.c .*.cmd .tmp_versions .*.flags .26

