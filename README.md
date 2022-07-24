Copyright (c) 2002-2010 Benoit Grégoire

# LibOFX
LibOFX library is an API designed to allow applications to very easily support OFX command responses, usually provided by financial institutions.  See http://www.ofx.net/ for details and specification. This project was first started as my end of degree project, with the objective to add OFX support to [GnuCash](https://www.gnucash.org/) If you can read French, the original project presentation is included in the doc directory.  I finally decided to make it into a generic library, so all OpenSource financial software can benefit.


LibOFX strives to achieve the following design goals:

* Simplicity: OFX is a VERY complex spec. However, few if any software needs all this complexity. The library tries to hide this complexity by "flattening" the data structures, doing timezone conversions, currency conversion, etc.
* Data directly usable from C, without conversion: A date is a C `time_t`, money is a float, strings are `char[]`, etc.
* C style interface: Although LibOFX is written in C++, it provides an interface usable transparently from both C and C++, using a single header file.

LibOFX was implemented directly from the full OFX 1.6 spec, and currently supports:
* Banking transactions and statements.
* Credit card and statements.
* Investment transactions.
* OFX 2.0

Future projects for libofx include:
* Header parsing
* DTD autodetection
* Currency conversion
* QIF import
* QIF export (integrated inside the library)
* OFX export

## Additional tools
In addition to the library, three utilities are included with libofx:

### ofxdump
ofxdump prints to stdout, in human-readable form, everything the library understands about a particular OFX response file, and sends errors to stderr.  It is a C++ code example and demo of the library (it uses every functions and every structures of LibOFX)\
usage: `ofxdump` _path_to_ofx_file_`/`_ofx_filename_

### ofx2qif
ofx2qif is a OFX "file" to QIF (Quicken Interchange Format) converter.  It was written as a C code example, and as a way for LibOFX to immediately provide something useful, as an incentive for people to try out the library.  It is not recommended that financial software use the output of this utility for OFX support.  The QIF file format is very primitive, and much information is lost.  The utility currently supports every transaction tags of the qif format except the address lines, and supports many of the tags of `!Account`. It should generate QIF files that will import successfully in just about every software with QIF support.
I do not plan on working on this utility much further, but I would be more than happy to accept contributions.\
usage: `ofx2qif` _path_to_ofx_file_`/`_ofx_filename_ `>` _output_filename_`.qif`

### ofxconnect
ofxconnect is a sample app to demonstrate & test new direct connect API'\s (try "make check" in the ofxconnect folder).  Read [ofxdump/README.privateserver](ofxdump/README.privateserver) first.

## Dependencies

### OpenSP
LibOFX is based on the excellent OpenSP library written by James Clark, and now part of the [OpenJADE](http://openjade.sourceforge.net/) project.  OpenSP by itself is not widely distributed.  OpenJADE 1.3.1 includes a version on OpenSP that will link, however, it has some major problems with LibOFX and isn't recommended.  Since LibOFX uses the generic interface to OpenSP, it should be compatible with all recent versions of OpenSP (It has been developed with OpenSP-1.5pre5).  LibOFX is written in C++, but provides a C style interface usable transparently from both C and C++ using a single include file.

LibOFX <==> OpenSP compatibility matrix:

| OpenSP version                                      | Status                         | Comments                                                                                   |
|-----------------------------------------------------|--------------------------------|--------------------------------------------------------------------------------------------|
| 1.3.4 (included in most distro with OpenJADE 1.3.1) | NOT COMPATIBLE                 | OpenSP doesn't parse OFX files correctly. LibOFX < 0.23 will probably only output garbage. |
| 1.4                                                 | NOT TESTED                     ||
| 1.5pre5                                             | COMPATIBLE with all versions   ||
| 1.5pre8                                             | COMPATIBLE with LibOFX >= 0.23 ||
| 1.5                                                 | COMPATIBLE with LibOFX >= 0.23 ||

#### On the SP_MULTI_BYTE 

LibOFX must know if OpenSP library was compiled with the `SP_MULTI_BYTE` defined. Our CMake build system makes best effort to detect the `SP_MULTI_BYTE`, as defined by OpenSP's `config.h`.  If you see things such as `SAEET` or `SAEET▄ù≡ù` instead of `STATEMENT` in ofxdump's output, it means that the library was misconfigured.
In such case you can force libofx to override such detected value by enabling the `DISABLE_OPENSP_MULTIBYTE` [CMake switch](#cmake-switches).

### Iconv
Used to convert the encoding, not required but very recommended.

### Libxml++
Required by the ofxconnect tool

### Libcurl
Required by the ofxconnect tool

## Building

### Using CMake build system

You can use CMake to build libofx. See the `CMakeLists.txt` in root folder for the currently required version.
CMake is designed so that the build process can be done in a separate directory. This is highly recommended for users and required for packagers.
To build LibOFX in the subdirectory `build/` type:

```shell
cmake -B build -S .   # add "-G Ninja" if ninja is installed (optional, see "CMake switches" section below)`
ccmake -B build -S .  # to change the configuration of the build process (optional, see "CMake switches" section below).
```

#### CMake switches
There are several CMake options available that can be enabled/disabled to change the set of features enabled or the compilation behavior:


| Switch                        |   Value    |     Default      | Purpose                                                                                                                                                                             | 
|-------------------------------|:----------:|:----------------:|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `-D BUILD_SHARED_LIBS`        | `ON`/`OFF` |       `ON`       | Whether to build the library dynamically (shared) or statically                                                                                                                     | Decides whether to build libofx as a shared or a static lib                                                                                                                        |  
| `-D ENABLE_ICONV`             | `ON`/`OFF` |       `ON`       | Enables ICONV support for encoding conversion                                                                                                                                       |
| `-D ENABLE_OFX2QIF`           | `ON`/`OFF` |       `ON`       | Enables OFX file to QIF (Quicken Interchange Format) file converter                                                                                                                 |
| `-D ENABLE_OFXDUMP`           | `ON`/`OFF` |       `ON`       | Enables ofxdump utility which prints, in human readable form, everything the library understands about a file                                                                       |
| `-D ENABLE_OFXCONNECT`        | `ON`/`OFF` |       `ON`       | Enables ofxconnect utility which allows to test OFX Direct Connect                                                                                                                  |
| `-D DISABLE_OPENSP_MULTIBYTE` | `ON`/`OFF` |      `OFF`       | Overrides the auto-detected value of the `SP_MULTI_BYTE` and forces it to 0. See [OpenSP notes](#opensp) on the purpose of it.                                                      |
| `-D CMAKE_BUILD_TYPE`         |  Various   | `RelWithDebInfo` | Chooses the build type; refer to CMake manual for a list of possible types                                                                                                          |
| `-D ENABLE_VCPKG_INTEGRATION` | `ON`/`OFF` |      `OFF`       | An alternative way to enable vcpkg integration. A attempt will be made to detect the location of the `vcpkg.cmake` toolchain file using `VCPKG_ROOT` environment variable.          |
| `-D CMAKE_TOOLCHAIN_FILE`     |   A path   |       None       | A path to the `[vcpkg root]/scripts/buildsystems/vcpkg.cmake` toolchain file, where `[vcpkg root]` is your clone of the vcpkg Git repository. Implies `ENABLE_VCPKG_INTEGRATION=ON` |

#### Vcpkg integration

vcpkg is a command-line package manager for C++, supported on macOS, Linux and Windows platforms. It is a preferred way of obtaining the dependencies due to its ability to deploy them according to a declarative `vcpkg.json` manifesto provided with the source code. This allows us to maintain a set of dependencies and their corresponding versions independently for each branch and automatically switching between them as needed. It also guarantees reproducibility of the dev environment and substantially reduces the effort required to set up a working environment.

Refer to the [Getting Started](https://github.com/microsoft/vcpkg#getting-started) guide on project's website on how to get started if you're not yet familiar with it.

In order to build LibOFX with it, simply enabling an additional switch in your CMake command should be enough:\
`-DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake`, where `[vcpkg root]` is your clone of the vcpkg Git repository. Alternatively, if you have `VCPKG_ROOT` environment variable set-up, you can simply enable `-D ENABLE_VCPKG_INTEGRATION=ON` switch to have it autodetect the location of the vcpkg toolchain file. See notes on the [CMake switches](#cmake-switches) available as well. 

Vcpkg will automatically set-up all the required optional dependencies, per the [CMake switches](#cmake-switches) enabled via the command line. 

### Using classic Autotools build system

Make sure you have the OpenSP library installed (libosp.so) before compiling.  You can grab it at http://openjade.sourceforge.net/. The recommended version is OpenSP 1.5.
If you have a version of OpenJade or OpenSP already installed before compiling OpenSP, make sure that you use ./configure --prefix=/usr to build OpenSP.  Otherwise, LibOFX will probably link with the older, incompatible version of OpenSP.

Then type:
`./autogen.sh` (For libofx cloned from repository)
`./configure` (For a distributed tarball)
`make`
And as root type:
`make install`

### Troubleshooting 

#### Getting "configure: error: unable to link a test program, is OpenSP installed?"

BE CAREFUL:  When you compile OpenSP, by default it will install in /usr/local.  On many versions of linux (Most RedHat, All Mandrake up to 8.3) `/usr/local/lib/` is not part of `/etc/ld.so.conf`.  In this case LibOFX will either:
1) Fail to link at all
2) Link with `libosp.so.0` (1.3.4, which probably got installed as part of OpenJade) and not work.
Even if you add `/usr/local/lib/` to `/etc/ld.so.conf`, LibOFX will probably still link with OpenSP 1.3.4 at runtime.  Symptoms will be complaints of many unclosed statements when running ofxdump on an ofx file.

I STRONGLY URGE YOU to avoid these problems and compile OpenSP using
`./configure --prefix=/usr`
If you really want to install OpenSP in `/usr/local/lib`, you must add `--with-opensp-libs=/usr/local/lib` to libofx's `./configure` or `./autogen.sh`

#### Problems compiling OpenSP 1.4

If you are getting errors like:
`../include/Message.h:157: `class OpenSP::Messenger' is inaccessible nsgmls.cxx:60: within this context)`

Try to change private to protected in file `OpenSP-1.4/nsgmls/RasEventHandler.h`, on line 105.

#### ofxdump does now work right.  It seems some values are cut in half, or one out of two letter is missing.

See the [On the SP_MULTI_BYTE](#on-the-sp_multi_byte) notes.

## API Documentation
Full documentation of the API and library internals generated using doxygen is available. For a quick start, you should learn all you need to know to get started by reading the `libofx.h` file in the INC directory, and `ofxdump.cpp` in the ofxdump directory.

## Get involved!
* Please note that despite a very detailed spec, OFX is by nature very hard to test.  I only have access to the specifications examples, and
my own bank (Desjardins).  But I need people to run as many OFX files from different banks as they can thru libofx, and report the result.
* This is my first attempt at writing an API.  I need comments from financial software writers about `inc/libofx.h`  What do YOU need?

Benoit Grégoire\
benoitg@coeus.ca

