#CC=gcc-2.96
#CXX=g++-2.96
CXXFLAGS = -g -O2 -Wall -pedantic
CFLAGS = -g -O2 -Wall -pedantic 

prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin
datadir = $(prefix)/share
incdir = $(prefix)/include
#libdir should be libdir = $(exec_prefix)/lib, but /usr/local/lib is often ommited from ld.conf
libdir = /usr/lib
