SUBDIRS = inc dtd lib doc . ofx2qif ofxdump 

docdir = ${prefix}/share/doc/libofx

doc_DATA = \
  AUTHORS \
  COPYING \
  INSTALL \
  NEWS \
  README \
  ChangeLog \
  totest.txt

EXTRA_DIST = \
  libofx.spec.in \
  libofx.spec \
  totest.txt \
  libofx.lsm \
  config

.PHONY: doc
doc:
	$(MAKE) -C doc doc
