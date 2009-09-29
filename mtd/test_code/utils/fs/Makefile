SUBDIRS = acls doio fs_inod fsstress fs_perms fsx-linux ftest inode lftest linktest openfile fs_di proc stream fs-bench racer acl fs_maim


all:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install PREFIX=../../bin ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
