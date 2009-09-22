SUBDIRS=touchscreen_apps_dir

all: $(SUBDIRS)

install:
	make -C touchscreen_apps install

touchscreen_apps_dir:
	make -C touchscreen_apps

clean:
	make -C touchscreen_apps clean
