all:
	$(MAKE) -C lib/
	$(MAKE) -C ofxdump/
	$(MAKE) -C ofx2qif/
	$(MAKE) -C doc/
static:
	$(MAKE) -C lib/ static STATIC=1
	$(MAKE) -C ofxdump/ static STATIC=1
	$(MAKE) -C ofx2qif/ static STATIC=1


rebuilddoc:
	$(MAKE) -C doc/ maintainer-clean
	$(MAKE) -C doc/

install:
	$(MAKE) -C lib/ install
	$(MAKE) -C ofxdump/ install
	$(MAKE) -C ofx2qif/ install

uninstall:
	$(MAKE) -C lib/ uninstall
	$(MAKE) -C ofxdump/ uninstall
	$(MAKE) -C ofx2qif/ uninstall

clean:
	rm -rf bin/*	
	rm -f *.o *~ core
	rm -f inc/*~
	$(MAKE) -C lib/ clean
	$(MAKE) -C ofxdump/ clean
	$(MAKE) -C ofx2qif/ clean
	$(MAKE) -C doc/ clean

maintainer-clean: clean
	$(MAKE) -C doc/ maintainer-clean

dist: rebuilddoc clean
ifdef version
	mkdir -p ../release	
	mkdir -p ../release/libofx-$(version)
	cp -r ./ ../release/libofx-$(version)
	cd ../release && tar -zcvf libofx-$(version).tar.gz libofx-$(version)
else
	@echo Error: You must specify the version.  Ex: make dist version=0.11
endif
