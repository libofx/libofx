SUBDIRS = 

docdir = ${prefix}/share/doc/libofx

doc_DATA = \
	implementation_notes.txt

EXTRA_DIST = \
	doxygen.conf \
	ofx_sample_files \
	implementation_notes.txt \
	tag_striper_test.txt \
	html

all:

html: doxygen.conf
	echo "html: " && pwd
	rm -rf html/ refman.pdf
	$(DOXYGEN) doxygen.conf

doc: doxygen.conf
	echo "doc: " && pwd && 	echo "distdir: " && echo $(distdir)
	rm -rf html/ refman.pdf
	$(DOXYGEN) doxygen.conf
#	$(MAKE) -C latex/ 
#	mv latex/refman.pdf ./refman.pdf

clean-local:
	echo "clean-local: " && pwd
	rm -rf latex/
	rm -f *~
	rm -f doxygen.log

maintainer-clean-local: clean-local
	echo "maintainer-clean-local: " && pwd
	rm -rf html refman.pdf

install-data-hook:
	mkdir -p $(srcdir)/html #Workaround to allow libofx-cvs user to install without doc.
	cp -rp $(srcdir)/html ${docdir}

uninstall-hook:
	rm -rf ${docdir}/html