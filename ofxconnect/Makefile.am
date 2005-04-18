bin_PROGRAMS = ofxconnect
ofxconnect_LDADD = $(top_builddir)/lib/libofx.la
ofxconnect_SOURCES = cmdline.h cmdline.c ofxconnect.cpp 
AM_CPPFLAGS = \
	-I${top_srcdir}/inc

if USE_GENGETOPT 
CLEANFILES = cmdline.c cmdline.h

cmdline.c cmdline.h: cmdline.ggo Makefile
	gengetopt --unamed-opts < $<

endif

EXTRA_DIST = cmdline.ggo

# See README.privateserver for details on this server and how to get
# the key needed to run this test.
TESTS = test-privateserver.sh 
