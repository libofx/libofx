#!/bin/sh
# Run this to generate all the initial makefiles, etc.

echo "Running mkdir -p config"
mkdir -p config
echo "Running libtoolize --force"
libtoolize --force || glibtoolize --force
echo "Running aclocal"
aclocal -I ./m4
echo "Running autoheader"
autoheader
echo "Running automake -a"
automake -a
echo "Running autoconf"
autoconf
echo "You can now run ./configure  $conf_flags $@ (potentially in a separate build directory)"
#./configure $conf_flags "$@"
