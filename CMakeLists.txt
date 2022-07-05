# SPDX-FileCopyrightText: 2009 Cristian Oneț <onet.cristian@gmail.com>
# SPDX-FileCopyrightText: 2022 Dawid Wróbel <me@dawidwrobel.com>

project(ofx)

set (LIBOFX_MAJOR_VERSION 0)
set (LIBOFX_MINOR_VERSION 10)
set (LIBOFX_MICRO_VERSION 5)
set (LIBOFX_VERSION_RELEASE_STRING ${LIBOFX_MAJOR_VERSION}.${LIBOFX_MINOR_VERSION}.${LIBOFX_MICRO_VERSION})

cmake_minimum_required(VERSION 3.12)
include(CMakeDependentOption)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

find_package(OpenSP REQUIRED)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(MSVC)

if(WIN32)
    set(_WIN32 1)
endif()

include_directories(
    inc
    ${CMAKE_CURRENT_BINARY_DIR}
    ${OpenSP_INCLUDE_DIRS}/OpenSP
)

add_definitions(
    -DMAKEFILE_DTD_PATH="${CMAKE_PREFIX_PATH}/share/libofx/dtd/"
)

file(GLOB COEFF_HDRS inc/libofx.h lib/*.hh )
file(GLOB SRC_SOURCES lib/*.cpp ${COEFF_HDRS})

add_library(ofx ${SRC_SOURCES})

FIND_LIBRARY( ICONV_LIBRARIES iconv )
if(MSVC)
    set_target_properties(ofx PROPERTIES OUTPUT_NAME "libofx")
endif(MSVC)
target_link_libraries(ofx ${OpenSP_LIBRARIES} ${ICONV_LIBRARIES})

# ofxconnect
find_package(CURL)

if (CURL_FOUND)
    set(HAVE_LIBCURL 1)
    find_package(PkgConfig)

    if(PkgConfig_FOUND)
        pkg_check_modules(LIBXMLPP IMPORTED_TARGET libxml++-2.6>=2.6)
    endif()
endif()

cmake_dependent_option(ENABLE_OFXCONNECT "Enable ofxconnect tool" ON
                       "CURL_FOUND;PkgConfig_FOUND;LIBXMLPP_FOUND" OFF)
add_feature_info("ofxconnect" ENABLE_OFXCONNECT "Enables ofxconnect utility which allows to test OFX Direct Connect")

if (ENABLE_OFXCONNECT)
    add_subdirectory( ofxconnect )
endif()

# ofxdump
option(ENABLE_OFXDUMP "Enable ofxdump tool" ON)
add_feature_info("ofxdump" ENABLE_OFXDUMP "Enables ofxconnect utility which prints, in human readable form, everything the library understands about a file")

if (ENABLE_OFXDUMP)
    add_subdirectory( ofxdump )
endif()

# ofx2qif
option(ENABLE_OFX2QIF "Enable ofx2qif tool" ON)
add_feature_info("ofx2qif" ENABLE_OFX2QIF "Enables OFX file to QIF (Quicken Interchange Format) file converter")

if (ENABLE_OFX2QIF)
    add_subdirectory( ofx2qif )
endif()


# create the libofx.pc pkg-config file out of the libofx.pc.in
configure_file("libofx.pc.in" "${CMAKE_CURRENT_BINARY_DIR}/libofx.pc")

# create the config.h file out of the config.h.cmake
configure_file("config.h.cmake" "${CMAKE_CURRENT_BINARY_DIR}/config.h")

install(TARGETS ofx ofxdump RUNTIME DESTINATION bin ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
install(FILES inc/libofx.h DESTINATION include/libofx)
install(FILES dtd/opensp.dcl DESTINATION "${CMAKE_PREFIX_PATH}/share/libofx/dtd")
install(FILES dtd/ofx160.dtd DESTINATION "${CMAKE_PREFIX_PATH}/share/libofx/dtd")
install(FILES dtd/ofx201.dtd DESTINATION "${CMAKE_PREFIX_PATH}/share/libofx/dtd")
install(FILES dtd/ofc.dtd DESTINATION "${CMAKE_PREFIX_PATH}/share/libofx/dtd")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libofx.pc" DESTINATION "${CMAKE_PREFIX_PATH}/lib/pkgconfig")

feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND
                DESCRIPTION "The following REQUIRED packages have not been found:")

feature_summary(WHAT OPTIONAL_PACKAGES_NOT_FOUND
                DESCRIPTION "The following OPTIONAL packages have not been found:")

feature_summary(WHAT ENABLED_FEATURES
                DESCRIPTION "The following features have been enabled:")

feature_summary(WHAT DISABLED_FEATURES
                DESCRIPTION "The following features have been disabled:")
