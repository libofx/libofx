#Makefile for ofxdump

include ../common.m

LPATH = -L../bin
IPATH = -I../inc -I/usr/include/OpenSP -I/usr/incude/sp/generic

binary = ofxdump

override CXXFLAGS += $(IPATH) $(LPATH)
ifdef STATIC
override CXXFLAGS+=-pg CFLAGS+=-pg
endif

objects = ofxdump.o

all: ../bin/$(binary)

static: $(objects) ../inc/libofx.h ../bin/libofx.a
	$(CXX) $(CXXFLAGS) -o ../bin/$(binary) $(objects) $(LPATH) ../bin/libofx.a -losp

../bin/$(binary): $(objects) ../inc/libofx.h
	$(CXX) $(CXXFLAGS) -o ../bin/$(binary) -lofx $(objects) $(LPATH)

install: 
	mkdir -p $(bindir)
	cp ../bin/$(binary) $(bindir)
uninstall:
	rm -f $(bindir)/$(binary) 

clean:
	rm -f *.o *~ core
