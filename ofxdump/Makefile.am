bin_PROGRAMS = ofxdump
ofxdump_LDADD = $(top_builddir)/lib/libofx.la
ofxdump_SOURCES = cmdline.h cmdline.c ofxdump.cpp 
AM_CPPFLAGS = \
	-I${top_srcdir}/inc

if USE_GENGETOPT 
CLEANFILES = cmdline.c cmdline.h

cmdline.c cmdline.h: cmdline.ggo Makefile
	gengetopt --unamed-opts < $<

endif
MAINTAINERCLEANFILES  = cmdline.c cmdline.h
EXTRA_DIST = cmdline.ggo

