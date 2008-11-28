## Process this file with automake to produce Makefile.in

INCLUDES = @GLIB_CFLAGS@ -I../src/

bin_PROGRAMS = irdadump 

irdadump_SOURCES = \
        main.c
irdadump_LDADD = @GLIB_LIBS@ ../src/libirdadump.a
