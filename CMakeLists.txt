# SPDX-FileCopyrightText: 2022 Dawid Wróbel <me@dawidwrobel.com>
# SPDX-License-Identifier: GPL-2.0-or-later

cmake_minimum_required(VERSION 3.12)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
include(CMakeDependentOption)
include(FeatureSummary)

# set-up tool features; this needs to be done early to let the vcpkg auto-download all the optional dependencies
option(ENABLE_ICONV "Build with ICONV" ON)
add_feature_info("iconv" ENABLE_ICONV "Enables ICONV support for encoding conversion")
if (ENABLE_ICONV)
    list(APPEND VCPKG_MANIFEST_FEATURES "iconv")
endif ()

option(ENABLE_OFXCONNECT "Enable ofxconnect tool" ON)
add_feature_info("ofxconnect" ENABLE_OFXCONNECT "Enables ofxconnect utility which allows to test OFX Direct Connect")
if (ENABLE_OFXCONNECT)
    list(APPEND VCPKG_MANIFEST_FEATURES "ofxconnect")
endif ()

option(ENABLE_OFXDUMP "Enable ofxdump tool" ON)
add_feature_info("ofxdump" ENABLE_OFXDUMP "Enables ofxdump utility which prints, in human readable form, everything the library understands about a file")
if (ENABLE_OFXDUMP)
    list(APPEND VCPKG_MANIFEST_FEATURES "ofxdump")
endif ()

option(ENABLE_OFX2QIF "Enable ofx2qif tool" ON)
add_feature_info("ofx2qif" ENABLE_OFX2QIF "Enables OFX file to QIF (Quicken Interchange Format) file converter")
if (ENABLE_OFX2QIF)
    list(APPEND VCPKG_MANIFEST_FEATURES "ofx2qif")
endif ()

# actually set up the project
project(libofx)

set(LIBOFX_MAJOR_VERSION 0)
set(LIBOFX_MINOR_VERSION 10)
set(LIBOFX_MICRO_VERSION 5)
set(LIBOFX_VERSION_RELEASE_STRING ${LIBOFX_MAJOR_VERSION}.${LIBOFX_MINOR_VERSION}.${LIBOFX_MICRO_VERSION})

# If no build type is set, use "Release with Debug Info"
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif (NOT CMAKE_BUILD_TYPE)

# locate required dependencies
find_package(OpenSP REQUIRED)

# locate optional dependencies
if (ENABLE_OFXCONNECT)
    find_package(CURL REQUIRED)
    if (CURL_FOUND)
        set(HAVE_LIBCURL 1)
    endif ()

    find_package(PkgConfig)
    if (PkgConfig_FOUND)
        pkg_check_modules(LIBXMLPP IMPORTED_TARGET libxml++-2.6>=2.6)
    endif ()
endif ()

if (ENABLE_ICONV)
    find_package(Iconv REQUIRED)
    if (Iconv_FOUND)
        set(HAVE_ICONV 1)
        include(CheckCXXSourceCompiles)
        set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} ${Iconv_INCLUDE_DIRS})
        set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} ${Iconv_LIBRARIES})
        check_cxx_source_compiles(
                "
            #include <iconv.h>
            int main(){
            const char* inbuf = 0;
            size_t inbytesleft = 0;
            char* outbuf = 0;
            size_t outbytesleft = 0;
            iconv(0, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
            return 0;
            }
            "
                HAVE_ICONV_CONST
        )
    endif ()
endif ()

# Build a shared lib by default, set this to OFF to build statically
option(BUILD_SHARED_LIBS "Decides whether to build ${CMAKE_PROJECT_NAME} as a shared or static library" ON)
add_feature_info("shared" BUILD_SHARED_LIBS "Enables compiling ${CMAKE_PROJECT_NAME} as a shared library (as opposed to statically)")

if (BUILD_SHARED_LIBS AND MSVC)
    add_compile_definitions(LIBOFX_DLL=1)
endif ()

# set up SP_MULTI_BYTE
option(DISABLE_OPENSP_MULTIBYTE "Force ${CMAKE_PROJECT_NAME} to compile with the assumption that OpenSP was NOT compiled with SP_MULTI_BYTE defined" OFF)

if (OpenSP_INCLUDE_DIR AND EXISTS "${OpenSP_INCLUDE_DIR}/config.h")
    file(STRINGS "${OpenSP_INCLUDE_DIR}/config.h" opensp_multibyte_str REGEX "^#define[\t ]+SP_MULTI_BYTE[ \t]+[01].*$")
    if (opensp_multibyte_str)
        string(REGEX REPLACE "^#define[\t ]+SP_MULTI_BYTE[ \t]+([01]).*$" "\\1" SP_MULTI_BYTE "${opensp_multibyte_str}")
        if (SP_MULTI_BYTE STREQUAL "")
            message(STATUS "SP_MULTI_BYTE detected but not set to any value, ignoring")
        else ()
            message(STATUS "SP_MULTI_BYTE detected to be ${SP_MULTI_BYTE}")
        endif ()
    endif ()
    unset(opensp_multibyte_str)
endif ()

if (NOT DISABLE_OPENSP_MULTIBYTE AND NOT SP_MULTI_BYTE)
    set(SP_MULTI_BYTE 1)
    message(STATUS "Could not detect SP_MULTI_BYTE, assuming to be 1")
elseif (DISABLE_OPENSP_MULTIBYTE)
    if (SP_MULTI_BYTE)
        message(WARNING "SP_MULTI_BYTE detected to be 1, but forced to be 0. If this is not indeded, it may lead to inability to process files correctly!")
    else ()
        message(STATUS "SP_MULTI_BYTE forced to 0")
    endif ()
    set(SP_MULTI_BYTE 0)
endif ()

# Disable CRT warnings
if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif ()

#TODO: once Autotools build stuff is gone, remove these and set them directly in libofx.pc.in itself
set(OPENSPLIBS ${OpenSP_LIBRARIES})
set(OPENSPINCLUDES ${OpenSP_INCLUDE_DIRS})
set(VERSION ${LIBOFX_VERSION_RELEASE_STRING})
set(prefix "${CMAKE_INSTALL_PREFIX}")
set(exec_prefix "\${prefix}")
set(libdir "\${prefix}/lib")
set(includedir "\${prefix}/include")

# create the libofx.h file out of the libofx.h.in
configure_file("inc/libofx.h.in" "${CMAKE_CURRENT_BINARY_DIR}/inc/libofx.h")

# create the config.h file out of the config.h.cmake
configure_file("config.h.cmake" "${CMAKE_CURRENT_BINARY_DIR}/config.h")

set(COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/inc ${CMAKE_CURRENT_BINARY_DIR})

# Handle the getopt in a cross-platform way
add_subdirectory(getopt)

# libofx itself
add_subdirectory(lib)

# ofxconnect
if (ENABLE_OFXCONNECT)
    add_subdirectory(ofxconnect)
endif ()

# ofxdump
if (ENABLE_OFXDUMP)
    add_subdirectory(ofxdump)
endif ()

# ofx2qif
if (ENABLE_OFX2QIF)
    add_subdirectory(ofx2qif)
endif ()

# set up vcpkg integration
option(ENABLE_VCPKG_INTEGRATION "Enable vcpkg integration" OFF)
add_feature_info("vcpkg" ENABLE_VCPKG_INTEGRATION "Enables integration with vcpkg, a C++ library manager")

if (ENABLE_VCPKG_INTEGRATION AND DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    message(STATUS "VCPKG found: $ENV{VCPKG_ROOT}")
    message(STATUS "Using VCPKG integration")
    message(STATUS "VCPKG_MANIFEST_FEATURES: ${VCPKG_MANIFEST_FEATURES}")
elseif (DEFINED CMAKE_TOOLCHAIN_FILE)
    message(STATUS "Using toolchain: ${CMAKE_TOOLCHAIN_FILE}")
    if (CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg.cmake")
        message(STATUS "Toolchain uses VCPKG integration")
        message(STATUS "VCPKG_MANIFEST_FEATURES: ${VCPKG_MANIFEST_FEATURES}")
        set(ENABLE_VCPKG_INTEGRATION ON)
    endif ()
endif ()

# create the libofx.pc pkg-config file out of the libofx.pc.in
configure_file("libofx.pc.in" "${CMAKE_BINARY_DIR}/libofx.pc")

install(DIRECTORY dtd DESTINATION share/libofx FILES_MATCHING PATTERN "*.dtd" PATTERN "*.dcl")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/inc/libofx.h" DESTINATION include/libofx)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libofx.pc" DESTINATION lib/pkgconfig)

feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND
        DESCRIPTION "The following REQUIRED packages have not been found:")

feature_summary(WHAT OPTIONAL_PACKAGES_NOT_FOUND
        DESCRIPTION "The following OPTIONAL packages have not been found:")

feature_summary(WHAT ENABLED_FEATURES
        DESCRIPTION "The following features have been enabled:")

feature_summary(WHAT DISABLED_FEATURES
        DESCRIPTION "The following features have been disabled:")
