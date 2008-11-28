CC := $(CROSS_COMPILE)gcc

SUBDIRS := video_app

all: $(SUBDIRS)

video_app:
	make -C video

install:
	make -C video install

clean:
	make -C video clean
