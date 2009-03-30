
all: fw.ihx

AS_BASE=/usr/local/8051
AS_BIN=$(AS_BASE)/asxxxx/asxmak/linux/exe

A51 = $(AS_BIN)/as8051
ASL = $(AS_BIN)/aslink

AFLAGS = -loswff

INCLUDES = config.inc firmware.inc

OBJECTS = main.rel device.rel function.rel modes.rel isoxfer.rel \
	usb_ep0.rel ctrlout.rel debug.rel

.SUFFIXES: .asm .rel

.asm.rel:
	$(A51) $(AFLAGS) $*.asm

main.rel:		main.asm $(INCLUDES)

device.rel:		device.asm $(INCLUDES)

function.rel:		function.asm $(INCLUDES)

modes.rel:		modes.asm $(INCLUDE)

isoxfer.rel:		isoxfer.asm $(INCLUDE)

usb_ep0.rel:		usb_ep0.asm $(INCLUDES) descr.inc

ctrlout.rel:		ctrlout.asm $(INCLUDES)

debug.rel:		debug.asm $(INCLUDES)

# following is just a hack so we can use fw.rel as first object
# (which is used by the linker to name the ihx output file)

fw.rel:
	@echo -e "XH2\nH\nM dummy\n" > fw.rel
	@touch fw.lst

fw.ihx: $(OBJECTS) fw.rel fw.lnk
	$(ASL) -n fw.rel -f fw.lnk

clean:
	rm -f *.rel *.lst *.sym *.map *.rst *.ihx

distclean:	clean
	rm -f *~

