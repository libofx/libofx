bin_PROGRAMS = ofxconnect
ofxconnect_LDADD = $(top_builddir)/lib/libofx.la @LIBCURL@ @LIBXMLPP_LIBS@ 
ofxconnect_SOURCES = cmdline.h cmdline.c nodeparser.cpp nodeparser.h ofxpartner.cpp ofxpartner.h ofxconnect.cpp 

AM_CPPFLAGS = \
	-I${top_srcdir}/inc \
	@LIBCURL_CPPFLAGS@ \
	@LIBXMLPP_CFLAGS@

if USE_GENGETOPT 
CLEANFILES = cmdline.c cmdline.h

cmdline.c cmdline.h: cmdline.ggo Makefile
	gengetopt --unamed-opts < $<

endif
MAINTAINERCLEANFILES  = cmdline.c cmdline.h

EXTRA_DIST = cmdline.ggo test-privateserver.sh

# See README.privateserver for details on this server and how to get
# the key needed to run this test.
TESTS = test-privateserver.sh 

