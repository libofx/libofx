bin_PROGRAMS = ofxconnect
ofxconnect_LDADD = $(top_builddir)/lib/libofx.la @LIBCURL@ 
ofxconnect_SOURCES = cmdline.h cmdline.c ofxconnect.cpp 

AM_CPPFLAGS = \
	-I${top_srcdir}/inc \
	@LIBCURL_CPPFLAGS@	

if USE_GENGETOPT 
CLEANFILES = cmdline.c cmdline.h

cmdline.c cmdline.h: cmdline.ggo Makefile
	gengetopt --unamed-opts < $<

endif

EXTRA_DIST = cmdline.ggo test-privateserver.sh

# See README.privateserver for details on this server and how to get
# the key needed to run this test.
TESTS = test-privateserver.sh 

