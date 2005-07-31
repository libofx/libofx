SUBDIRS = inc dtd lib doc . ofx2qif ofxdump ofxconnect 

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
  libofx.lsm

.PHONY: doc
doc:
	$(MAKE) -C doc doc

rpm:    $(PACKAGE).spec dist
	rpmbuild="rpm" && \
	if [ `rpm --version | awk '{ print $$3 }'` > /dev/null ]; then rpmbuild="rpmbuild"; fi && \
	$$rpmbuild -ta $(PACKAGE)-$(VERSION).tar.gz
