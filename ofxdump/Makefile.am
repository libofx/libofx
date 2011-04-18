bin_PROGRAMS = ofxdump
ofxdump_LDADD = $(top_builddir)/lib/libofx.la
ofxdump_SOURCES = cmdline.h cmdline.c ofxdump.cpp
dist_man_MANS = ofxdump.1

AM_CPPFLAGS = \
	-I${top_builddir}/inc

if USE_GENGETOPT 
CLEANFILES = cmdline.c cmdline.h

cmdline.c cmdline.h: cmdline.ggo Makefile
	gengetopt --unamed-opts < $<

endif

ofxdump.1: ofxdump.cpp $(top_srcdir)/configure.in
	$(MAKE) $(AM_MAKEFLAGS) ofxdump$(EXEEXT)
	help2man --output=ofxdump.1 ./ofxdump$(EXEEXT)

MAINTAINERCLEANFILES  = cmdline.c cmdline.h
EXTRA_DIST = cmdline.ggo

