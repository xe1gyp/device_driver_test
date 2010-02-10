.PHONY: libs clean install sources

libaio_sources=$(PWD)/libaio-0.3.106
prefix=$(PWD)/usr
CC=arm-none-linux-gnueabi-gcc
ARCH=arm
SOURCES=usb.c usbstring.c

gadget:libs $(SOURCES)
	@$(CC) -Wall -DAIO -g -o usb $(SOURCES) -lpthread -laio -I $(KDIR)/2.6_kernel/include/ -I $(prefix)/include -L $(prefix)/lib -L $(KDIR)/lib -I $(KDIR)/arch/arm/include/

libs:
	@$(MAKE) -C $(libaio_sources) CC=$(CC) ARCH=$(ARCH) prefix=$(prefix) install

install:
	@cp -v $(PWD)/usb $(prefix)/lib/libaio.so.1 ../../bin

clean:
	@rm -vrf $(prefix) $(PWD)/usb
	@$(MAKE) -C $(libaio_sources) clean
