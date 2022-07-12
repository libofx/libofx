# SPDX-FileCopyrightText: 2009 Cristian Oneț <onet.cristian@gmail.com>
# SPDX-FileCopyrightText: 2022 Dawid Wróbel <me@dawidwrobel.com>

project(ofx)

set (LIBOFX_MAJOR_VERSION 0)
set (LIBOFX_MINOR_VERSION 10)
set (LIBOFX_MICRO_VERSION 5)
set (LIBOFX_VERSION_RELEASE_STRING ${LIBOFX_MAJOR_VERSION}.${LIBOFX_MINOR_VERSION}.${LIBOFX_MICRO_VERSION})

cmake_minimum_required(VERSION 3.12)

# If no build type is set, use "Release with Debug Info"
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif(NOT CMAKE_BUILD_TYPE)

include(CMakeDependentOption)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

find_package(OpenSP REQUIRED)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(MSVC)

if(WIN32)
    set(_WIN32 1)
endif()

# TODO: HAVE_GCC_VISIBILITY_EXTS vs CMAKE_CXX_VISIBILITY_PRESET
# create the libofx.h file out of the libofx.h.in
configure_file("inc/libofx.h.in" "${CMAKE_CURRENT_BINARY_DIR}/inc/libofx.h")

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
add_subdirectory( ofxconnect )

# ofxdump
add_subdirectory( ofxdump )

# ofx2qif
add_subdirectory( ofx2qif )

# create the libofx.pc pkg-config file out of the libofx.pc.in
configure_file("libofx.pc.in" "${CMAKE_CURRENT_BINARY_DIR}/libofx.pc")

# create the config.h file out of the config.h.cmake
configure_file("config.h.cmake" "${CMAKE_CURRENT_BINARY_DIR}/config.h")

install(TARGETS ofx ofxdump RUNTIME DESTINATION bin ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
install(DIRECTORY dtd DESTINATION share/libofx FILES_MATCHING PATTERN "*.dtd" PATTERN "*.dcl")
install(FILES inc/libofx.h DESTINATION include/libofx)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libofx.pc" DESTINATION lib/pkgconfig)

feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND
                DESCRIPTION "The following REQUIRED packages have not been found:")

feature_summary(WHAT OPTIONAL_PACKAGES_NOT_FOUND
                DESCRIPTION "The following OPTIONAL packages have not been found:")

feature_summary(WHAT ENABLED_FEATURES
                DESCRIPTION "The following features have been enabled:")

feature_summary(WHAT DISABLED_FEATURES
                DESCRIPTION "The following features have been disabled:")
