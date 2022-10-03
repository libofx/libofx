# SPDX-FileCopyrightText: 2022 Dawid Wróbel <me@dawidwrobel.com>
# SPDX-License-Identifier: GPL-2.0-or-later

cmake_minimum_required(VERSION 3.12)

if (${CMAKE_VERSION} VERSION_LESS 3.25.0)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
endif ()

include(GNUInstallDirs)
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

# This is the project version
set(LIBOFX_MAJOR_VERSION 0)
set(LIBOFX_MINOR_VERSION 10)
set(LIBOFX_MICRO_VERSION 8)
set(LIBOFX_VERSION_RELEASE_STRING ${LIBOFX_MAJOR_VERSION}.${LIBOFX_MINOR_VERSION}.${LIBOFX_MICRO_VERSION})

# This is the library version (SONAME) so affects ABI
# LIBOFX_LIBRARY_SONAME must be changed when ABI is broken (backwards compat)
# LIBOFX_LIBRARY_VERSION must be changed if just adding new functions and such (backwards compat not broken)
# Note that these are used by write_basic_package_version_file() to generate LibOFXConfigVersion.cmake
set(LIBOFX_LIBRARY_SONAME 7)
set(LIBOFX_LIBRARY_VERSION ${LIBOFX_LIBRARY_SONAME}.0.3)

# If no build type is set, use "Release with Debug Info"
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif (NOT CMAKE_BUILD_TYPE)
include(CTest)

# locate required dependencies
find_package(OpenSP)
set_package_properties(OpenSP PROPERTIES TYPE REQUIRED PURPOSE "The underlying library that the LibOFX builds upon, we cannot build without it!")

# locate optional dependencies
if (ENABLE_OFXCONNECT)
    find_package(CURL)
    set_package_properties(CURL PROPERTIES TYPE REQUIRED PURPOSE "ENABLE_OFXCONNECT is ON, so we need curl for the ofxconnect tool to perform downloading of the statements")
    if (CURL_FOUND)
        set(HAVE_LIBCURL 1)
    endif ()

    find_package(PkgConfig)
    set_package_properties(PkgConfig PROPERTIES TYPE REQUIRED PURPOSE "ENABLE_OFXCONNECT is ON, so we need PkgConfig to locate the libxml++ library")

    if (PkgConfig_FOUND)
        pkg_check_modules(LIBXMLPP REQUIRED IMPORTED_TARGET libxml++-2.6>=2.6)
        #TODO: if not found, handle the feature_summary by hand somehow
    endif ()
endif ()

if (ENABLE_ICONV)
    find_package(Iconv)
    set_package_properties(Iconv PROPERTIES TYPE REQUIRED PURPOSE "ENABLE_ICONV is on, so we need Iconv to build")

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
set(FORCE_OPENSP_MULTIBYTE AUTO CACHE STRING "Whether to let FindOpenSP CMake module auto-detect the SP_MULTIBYTE value (leave 'AUTO'), or to override it with 'ON' or 'OFF'")
set_property(CACHE FORCE_OPENSP_MULTIBYTE PROPERTY STRINGS AUTO ON OFF)

# CheckCXXSymbolExists returns "" if symbol is not found
if (NOT OpenSP_MULTI_BYTE)
    set(OpenSP_MULTI_BYTE 0)
endif()

message(CHECK_START "Setting up the SP_MULTI_BYTE")
set(SP_MULTI_BYTE OpenSP_MULTI_BYTE)
if (NOT FORCE_OPENSP_MULTIBYTE STREQUAL "AUTO")
    if (NOT FORCE_OPENSP_MULTIBYTE EQUAL OpenSP_MULTI_BYTE)
        message(CHECK_PASS "detected by FindOpenSP to be ${OpenSP_MULTI_BYTE}, but overriding to ${FORCE_OPENSP_MULTIBYTE}, as set by FORCE_OPENSP_MULTIBYTE. If this is not indeded, it can cause wide character processing issues!")
        set(SP_MULTI_BYTE ${FORCE_OPENSP_MULTIBYTE})
    else ()
        message(CHECK_PASS "detected by FindOpenSP to be ${OpenSP_MULTI_BYTE}, with FORCE_OPENSP_MULTIBYTE also set to ${FORCE_OPENSP_MULTIBYTE}")
    endif ()
else ()
    message(CHECK_PASS "detected by FindOpenSP to be ${OpenSP_MULTI_BYTE}. Use FORCE_OPENSP_MULTIBYTE to set a different value")
endif ()

# Disable CRT warnings
if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif ()

# create the libofx.h file out of the libofx.h.in
configure_file("inc/libofx.h.in" "${CMAKE_CURRENT_BINARY_DIR}/inc/libofx.h")

# create the config.h file out of the config.h.cmake
configure_file("config.h.cmake" "${CMAKE_CURRENT_BINARY_DIR}/config.h")

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

# TODO: wire up docs
# TODO: wire up tests

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

# TODO: once Autotools build stuff is gone, remove these and set them directly in libofx.pc.in itself
set(OPENSPLIBS ${OpenSP_LIBRARIES})
set(OPENSPINCLUDES ${OpenSP_INCLUDE_DIRS})
set(VERSION ${LIBOFX_VERSION_RELEASE_STRING})
set(prefix "${CMAKE_INSTALL_PREFIX}")
set(exec_prefix "\${prefix}")
set(libdir "\${prefix}/${CMAKE_INSTALL_LIBDIR}")
set(includedir "\${prefix}/${CMAKE_INSTALL_INCLUDEDIR}")

# create the libofx.pc pkg-config file out of the libofx.pc.in
configure_file("libofx.pc.in" "${CMAKE_BINARY_DIR}/libofx.pc")

include(CMakePackageConfigHelpers)
configure_package_config_file(LibOFXConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/LibOFXConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libofx
        PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR CMAKE_INSTALL_BINDIR CMAKE_INSTALL_DATADIR
        )
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/LibOFXConfigVersion.cmake
        VERSION ${LIBOFX_LIBRARY_VERSION}
        COMPATIBILITY SameMajorVersion
        )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/LibOFXConfig.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/LibOFXConfigVersion.cmake
            ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/FindOpenSP.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libofx
        )

install(DIRECTORY dtd DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/libofx FILES_MATCHING PATTERN "*.dtd" PATTERN "*.dcl")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/inc/libofx.h" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libofx)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libofx.pc" DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
install(EXPORT LibOFXTargets
        FILE LibOFXTargets.cmake
        NAMESPACE libofx::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libofx
        )

feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND
        DESCRIPTION "The following REQUIRED packages have not been found:")

feature_summary(WHAT OPTIONAL_PACKAGES_NOT_FOUND
        DESCRIPTION "The following OPTIONAL packages have not been found:")

feature_summary(WHAT ENABLED_FEATURES
        DESCRIPTION "The following features have been enabled:")

feature_summary(WHAT DISABLED_FEATURES
        DESCRIPTION "The following features have been disabled:")

# ############################################################
# Package creation rules

if(UNIX)
  set(CPACK_GENERATOR "TGZ")
  set(CPACK_SOURCE_GENERATOR "TGZ")
endif()

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "This library is a client and file format implementation for the financial format OFX (Open Financial Exchange)")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_VERSION_MAJOR ${LIBOFX_MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${LIBOFX_MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${LIBOFX_MICRO_VERSION})
set(CPACK_SOURCE_IGNORE_FILES "/build.*/;/\\\\.git/;.*~;\\\\.vscode;\\\\.github")

include (CPack)
