SUBDIRS = inc dtd lib doc . ofx2qif ofxdump 

docdir = ${prefix}/share/doc/libofx

doc_DATA = \
  AUTHORS \
  COPYING \
  INSTALL \
  NEWS \
  README \
  ChangeLog \
  totest.txt \
  known_bugs.txt
  
EXTRA_DIST = \
  libofx.spec.in \
  libofx.spec \
  totest.txt \
  known_bugs.txt \
  libofx.lsm \
  config

.PHONY: doc
doc:
	$(MAKE) -C doc doc

#dist-hook: doc
#	echo "dist-hook: " && pwd
