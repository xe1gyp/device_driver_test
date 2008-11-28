REQUIRED_LINKS=include kernel fs
REQUIRED_FILES=compr_lzari.c compr_lzo.c compr_rtime.c compr_rubin.c compr_rubin.h compr_zlib.c

all:
	$(foreach b, $(REQUIRED_LINKS), (if [ ! -h $(b) ]; then ln -s $(KDIR)/$(b) $(b); fi;);) 
	if [ ! -h utils ]; then ln -s mtd-utils-1.0.0 utils; fi
	make -C utils
	
clean:
	if [ -h utils ]; then make clean -C utils; fi
	$(foreach b, $(REQUIRED_FILES), (if [ -h fs/jffs2/$(b) ]; then rm -vf fs/jffs2/$(b); fi;);) 
	$(foreach b, $(REQUIRED_LINKS), (rm -f $(b));) 
	rm -rf utils

install:
	make -C utils install
