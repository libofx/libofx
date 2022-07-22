# SPDX-FileCopyrightText: 2022 Dawid Wróbel <me@dawidwrobel.com>
# SPDX-License-Identifier: GPL-2.0-or-later

project(libofx)

set (LIBOFX_MAJOR_VERSION 0)
set (LIBOFX_MINOR_VERSION 10)
set (LIBOFX_MICRO_VERSION 5)
set (LIBOFX_VERSION_RELEASE_STRING ${LIBOFX_MAJOR_VERSION}.${LIBOFX_MINOR_VERSION}.${LIBOFX_MICRO_VERSION})

cmake_minimum_required(VERSION 3.12)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include(CMakeDependentOption)
include(FeatureSummary)

# If no build type is set, use "Release with Debug Info"
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif(NOT CMAKE_BUILD_TYPE)

# Build a shared lib by default, set this to OFF to build statically
option(BUILD_SHARED_LIBS "Decides whether to build ${CMAKE_PROJECT_NAME} as a shared or static library" ON)
add_feature_info("shared" BUILD_SHARED_LIBS "Enables compiling ${CMAKE_PROJECT_NAME} as a shared library (as opposed to statically)")

# locate dependencies
find_package(CURL)

if (CURL_FOUND)
    set(HAVE_LIBCURL 1)
    find_package(PkgConfig)

    if(PkgConfig_FOUND)
        pkg_check_modules(LIBXMLPP IMPORTED_TARGET libxml++-2.6>=2.6)
    endif()
endif()

find_package(OpenSP REQUIRED)

# set up SP_MULTI_BYTE
option(DISABLE_OPENSP_MULTIBYTE "Force ${CMAKE_PROJECT_NAME} to compile with the assumption that OpenSP was NOT compiled with SP_MULTI_BYTE defined" OFF)

if (OpenSP_INCLUDE_DIR AND EXISTS "${OpenSP_INCLUDE_DIR}/config.h")
    file(STRINGS "${OpenSP_INCLUDE_DIR}/config.h" opensp_multibyte_str REGEX "^#define[\t ]+SP_MULTI_BYTE[ \t]+[01].*$")
    if (opensp_multibyte_str)
        string(REGEX REPLACE "^#define[\t ]+SP_MULTI_BYTE[ \t]+([01]).*$" "\\1" SP_MULTI_BYTE "${opensp_multibyte_str}")
        if(SP_MULTI_BYTE STREQUAL "")
            message(STATUS "SP_MULTI_BYTE detected but not set to any value, ignoring")
        else()
            message(STATUS "SP_MULTI_BYTE detected to be ${SP_MULTI_BYTE}")
        endif()
    endif()
    unset(opensp_multibyte_str)
endif()

if (NOT DISABLE_OPENSP_MULTIBYTE AND NOT SP_MULTI_BYTE)
    set(SP_MULTI_BYTE 1)
    message(STATUS "Could not detect SP_MULTI_BYTE, assuming to be 1")
elseif(DISABLE_OPENSP_MULTIBYTE)
    if (SP_MULTI_BYTE)
        message(WARNING "SP_MULTI_BYTE detected to be 1, but forced to be 0. If this is not indeded, it may lead to inability to process files correctly!")
    else()
        message(STATUS "SP_MULTI_BYTE forced to 0")
    endif()
    set(SP_MULTI_BYTE 0)
endif()

#TODO: once Autotools build stuff is gone, remove these and set them directly in libofx.pc.in itself
set(OPENSPLIBS ${OpenSP_LIBRARIES})
set(OPENSPINCLUDES ${OpenSP_INCLUDE_DIRS})
set(VERSION ${LIBOFX_VERSION_RELEASE_STRING})
set(prefix "${CMAKE_INSTALL_PREFIX}")
set(exec_prefix "\${prefix}")
set(libdir "\${prefix}/lib")
set(includedir "\${prefix}/include")

find_package(Iconv)

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
endif()

# TODO: HAVE_GCC_VISIBILITY_EXTS vs CMAKE_CXX_VISIBILITY_PRESET
# create the libofx.h file out of the libofx.h.in
configure_file("inc/libofx.h.in" "${CMAKE_CURRENT_BINARY_DIR}/inc/libofx.h")

# create the config.h file out of the config.h.cmake
configure_file("config.h.cmake" "${CMAKE_CURRENT_BINARY_DIR}/config.h")

set(COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/inc ${CMAKE_CURRENT_BINARY_DIR})

# Handle the getopt in a cross-platform way
add_subdirectory( getopt)

# libofx itself
add_subdirectory( lib )

# ofxconnect
add_subdirectory( ofxconnect )

# ofxdump
add_subdirectory( ofxdump )

# ofx2qif
add_subdirectory( ofx2qif )

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
