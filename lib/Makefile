#Makefile for the libofx library

include ../common.m

LPATH =
IPATH = -I../inc -I/usr/include/OpenSP -I/usr/incude/sp/generic

override CXXFLAGS += -fPIC $(IPATH) $(LPATH) -D MAKEFILE_DTD_PATH=\"$(datadir)/libofx/dtd/\"
ifdef STATIC
override CXXFLAGS+=-pg
override CFLAGS+=-pg
endif
objects = messages.o ofx_utilities.o ofx_preproc.o ofx_proc_rs.o ofx_sgml.o

library = ../bin/libofx.so.1.0
libofx: $(library)

static: ../bin/libofx.a $(library)

../bin/libofx.a: $(objects)
	$(AR) rcs ../bin/libofx.a $(objects)

$(library):  $(objects)
	$(CXX) $(CXXFLAGS) -shared -o ../bin/libofx.so.1.0 -losp  $(objects)
#	ld -shared -fPIC -soname libofx.so.1 -o ../bin/libofx.so.1.0 -losp $(objects)
	ln -sf ../bin/libofx.so.1.0 ../bin/libofx.so.1
	ln -sf ../bin/libofx.so.1 ../bin/libofx.so


messages.o:   ofx_utilities.h messages.h

ofx_utilities.o:   messages.h ofx_utilities.h

ofx_proc_rs.o:  ../inc/libofx.h messages.h ofx_error_msg.h ofx_utilities.h ofx_proc_rs.h

ofx_sgml.o:  ../inc/libofx.h ofx_utilities.h messages.h ofx_proc_rs.h ofx_sgml.h

ofx_preproc.o: ofx_preproc.h messages.h ofx_sgml.h

install: ../dtd/ofx160.dtd ../dtd/ofx201.dtd
	mkdir -p $(datadir)/libofx/dtd
	cp ../dtd/ofx160.dtd $(datadir)/libofx/dtd
	cp ../dtd/ofx201.dtd $(datadir)/libofx/dtd
	mkdir -p $(incdir)/
	cp ../inc/libofx.h $(incdir)/
	mkdir -p $(libdir)
	cp ../bin/libofx.so.1.0 $(libdir)
	ln -sf $(libdir)/libofx.so.1.0 $(libdir)/libofx.so.1
	ln -sf $(libdir)/libofx.so.1 $(libdir)/libofx.so
	ldconfig

uninstall:
	rm -f $(datadir)/libofx/dtd/ofx160.dtd
	rm -f $(datadir)/libofx/dtd/ofx201.dtd	
	rmdir $(datadir)/libofx/dtd
	rmdir $(datadir)/libofx
	rm -f $(incdir)/libofx.h
	rm -f $(libdir)/libofx.so.1.0 
	rm -f $(libdir)/libofx.so.1 
	rm -f $(libdir)/libofx.so
	ldconfig 

clean:
	rm -f *.o *~ core *.so
	rm -rf .deps
	rm -rf .libs
