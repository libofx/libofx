ACLOCAL_AMFLAGS = -I m4

if BUILD_OFX2QIF
  MAYBE_OFX2QIF = ofx2qif
endif

if BUILD_OFXDUMP
  MAYBE_OFXDUMP = ofxdump
endif

if BUILD_OFXCONNECT
  MAYBE_OFXCONNECT = ofxconnect
endif

DIST_SUBDIRS = m4 inc dtd lib doc . getopt ofx2qif ofxdump ofxconnect
SUBDIRS = m4 inc dtd lib doc . $(MAYBE_OFX2QIF) $(MAYBE_OFXDUMP) $(MAYBE_OFXCONNECT)

doc_DATA = \
  AUTHORS \
  COPYING \
  INSTALL \
  NEWS \
  README \
  ChangeLog \
  totest.txt

EXTRA_DIST = \
  CMakeLists.txt \
  cmake/modules/FindOpenSP.cmake \
  config.h.cmake \
  LibOFXConfig.cmake.in \
  libofx.spec.in \
  libofx.spec \
  libofx.pc \
  totest.txt \
  libofx.lsm.in \
  libofx.lsm

pkgconfigdir=$(libdir)/pkgconfig
pkgconfig_DATA=libofx.pc

.PHONY: doc
doc:
	$(MAKE) -C doc doc

rpm:    $(PACKAGE).spec dist
	rpmbuild="rpm" && \
	if [ `rpm --version | awk '{ print $$3 }'` > /dev/null ]; then rpmbuild="rpmbuild"; fi && \
	$$rpmbuild -ta $(PACKAGE)-$(VERSION).tar.gz
