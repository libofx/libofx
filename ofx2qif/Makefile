#Makefile for ofx2qif

include ../common.m

LPATH = -L../bin
IPATH = -I../inc -I/usr/include/OpenSP -I/usr/incude/sp/generic

binary = ofx2qif

override CFLAGS += $(IPATH) $(LPATH)
ifdef STATIC
override CXXFLAGS+=-pg
override CFLAGS+=-pg
endif

objects = ofx2qif.o

all: ../bin/$(binary)

static: $(objects) ../inc/libofx.h ../bin/libofx.a
	$(CXX) $(CXXFLAGS) -o ../bin/$(binary) $(objects) $(LPATH) ../bin/libofx.a -losp

../bin/$(binary): $(objects) ../inc/libofx.h
	$(CC) $(CFLAGS) -o ../bin/$(binary) -lofx -lstdc++ $(objects) $(LPATH)

install:
	mkdir -p $(bindir)
	cp ../bin/$(binary) $(bindir)
uninstall:
	rm -f $(bindir)/$(binary) 
clean:
	rm -f *.o *~ core
