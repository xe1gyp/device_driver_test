SUBDIRS=keypad_apps_dir

all: $(SUBDIRS)

install:
	make -C keypad_apps install

keypad_apps_dir:
	make -C keypad_apps

clean:
	make -C keypad_apps clean
