bin_PROGRAMS = ofxdump
ofxdump_LDADD = $(top_builddir)/lib/libofx.la
ofxdump_SOURCES = cmdline.h cmdline.c ofxdump.cpp
dist_man_MANS = ofxdump.1

AM_CPPFLAGS = \
	-I${top_builddir}/inc \
	'-DCMDLINE_PARSER_PACKAGE="ofxdump"'

if USE_GENGETOPT 
CLEANFILES = cmdline.c cmdline.h

cmdline.c cmdline.h: cmdline.ggo Makefile
	gengetopt --unamed-opts < $<

endif

ofxdump.1: ofxdump.cpp $(top_srcdir)/configure.ac
	$(MAKE) $(AM_MAKEFLAGS) ofxdump$(EXEEXT)
if HAVE_HELP2MAN
	$(HELP2MAN) -n 'Dump content of OFX files as human-readable text' -N --output=ofxdump.1 ./ofxdump$(EXEEXT)
else
	echo "*** No man page available because of missing help2man tool" > $@
endif

MAINTAINERCLEANFILES  = cmdline.c cmdline.h
EXTRA_DIST = cmdline.ggo

# Run all our example files through ofxdump to verify we can parse
# them without failure
TESTS = \
 ../doc/ofx_sample_files/another-example.xml \
 ../doc/ofx_sample_files/example-listing1.xml \
 ../doc/ofx_sample_files/example-listing2.xml \
 ../doc/ofx_sample_files/ofx_spec160_stmtrs_example.sgml \
 ../doc/ofx_sample_files/ofx_spec201_stmtrs_example.xml \
 ../doc/ofx_sample_files/ofx-2.1.1-sect-13.12.ofx \
 ../doc/ofx_sample_files/ofx-2.1.1-sect-13.13.ofx \
 ../doc/ofx_sample_files/ofx-2.1.1-sect-16.5.3.2.ofx \
 ../doc/ofx_sample_files/ofx-2.1.1-sect-16.5.4.2.ofx \
 ../doc/ofx_sample_files/ofx-2.1.1-sect-16.5.6.2.ofx \
 ../doc/ofx_sample_files/ofx-2.1.1-sect-16.5.6.5.ofx \
 ../doc/ofx_sample_files/ofx-2.1.1-sect-8.5.5.ofx \
 ../doc/ofx_sample_files/failing-ofx-file1.xml \
 ../doc/ofx_sample_files/failing-ofx-file2.xml

# To ensure we are not just always green, test that two bogus files
# actually fail the ofxdump run.
# The "sect-13.13" example seems to be not yet implemented here, though.
XFAIL_TESTS = \
 ../doc/ofx_sample_files/failing-ofx-file1.xml \
 ../doc/ofx_sample_files/failing-ofx-file2.xml \
 ../doc/ofx_sample_files/ofx-2.1.1-sect-13.13.ofx

TEST_EXTENSIONS = .xml .sgml .ofx
OFX_LOG_COMPILER = ./ofxdump
# Must set the correct path to the DTD as env variable
AM_TESTS_ENVIRONMENT = OFX_DTD_PATH='$(top_srcdir)/dtd'; export OFX_DTD_PATH;
XML_LOG_COMPILER = $(OFX_LOG_COMPILER)
SGML_LOG_COMPILER = $(OFX_LOG_COMPILER)
