html:
	$(MAKE) -C ./ maintainer-clean
	doxygen doxygen.cfg
#	$(MAKE) -C latex/
#	mv latex/refman.pdf ./refman.pdf

clean:
	rm -rf latex/
	rm -f *~
	rm -f doxygen.log

maintainer-clean: clean
	rm -rf html/ refman.pdf
