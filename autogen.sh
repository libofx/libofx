#!/bin/sh
# Run this to generate all the initial makefiles, etc.

echo "Running aclocal"
aclocal
echo "Running autoconf"
autoconf
echo "Running autoheader"
autoheader
echo "Running mkdir -p config"
mkdir -p config
echo "Running automake -a"
automake -a
echo "Running ./configure --enable-maintainer-mode"
./configure --enable-maintainer-mode $conf_flags "$@"
