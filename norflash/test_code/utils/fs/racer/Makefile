TARGETS=dir_create.sh dir_test.sh file_concat.sh file_create.sh file_link.sh file_list.sh file_rename.sh file_rm.sh file_symlink.sh racer.sh

all:
 
install:
	@set -e; for i in $(TARGETS) ; do ln -f $$i ../../../bin/$$i ; done

clean:
