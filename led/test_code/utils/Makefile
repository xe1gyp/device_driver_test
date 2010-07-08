SUBDIRS=led_apps_dir

all: $(SUBDIRS)

install:
	make -C led_apps install

led_apps_dir:
	make -C led_apps

clean:
	make -C led_apps clean
