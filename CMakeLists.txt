# SPDX-FileCopyrightText: 2009 Cristian Oneț <onet.cristian@gmail.com>

project(ofx)

set (VERSION_MAJOR 0)
set (VERSION_MINOR 10)
set (VERSION_PATCH 5)
set (VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

cmake_minimum_required(VERSION 2.6)

list(APPEND CMAKE_MODULE_PATH  ${CMAKE_SOURCE_DIR})
find_package(OpenSP REQUIRED)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif(MSVC)

include_directories(
    inc
    ${OPENSP_INCLUDES}
    ${CMAKE_INSTALL_PREFIX}/include
)

add_definitions(
    -D__WIN32__
    -DMAKEFILE_DTD_PATH="${CMAKE_PREFIX_PATH}/share/libofx/dtd/"
)

file(GLOB COEFF_HDRS inc/libofx.h lib/*.hh )
file(GLOB SRC_SOURCES lib/*.cpp ${COEFF_HDRS})


# We would like it to be shared, but libofx does not export symbols manually
# thus it causes troubles on MSVC
#add_library(ofx SHARED ${SRC_SOURCES})
add_library(ofx ${SRC_SOURCES})

FIND_LIBRARY( ICONV_LIBRARIES iconv )
if(MSVC)
    set_target_properties(ofx PROPERTIES OUTPUT_NAME "libofx")
endif(MSVC)
target_link_libraries(ofx ${OPENSP_LIBRARIES} ${ICONV_LIBRARIES})

######### add a utility function so that we can test ofx files ##########
set(ofxdump_SRCS
    ofxdump/cmdline.c
    ofxdump/ofxdump.cpp 
)

if(MSVC)
    set(ofxdump_SRCS ${ofxdump_SRCS} lib/messages.cpp lib/getopt.c lib/getopt1.c)
endif(MSVC)

add_definitions(-DCMDLINE_PARSER_PACKAGE=\"ofxdump\" -DCMDLINE_PARSER_PACKAGE_NAME=\"ofxdump\" -DCMDLINE_PARSER_VERSION=\"${VERSION}\")
add_executable(ofxdump ${ofxdump_SRCS})
target_link_libraries(ofxdump ofx)

configure_file("libofx.pc.in" "${CMAKE_CURRENT_BINARY_DIR}/libofx.pc")

install(TARGETS ofx ofxdump RUNTIME DESTINATION bin ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
install(FILES inc/libofx.h DESTINATION include/libofx)
install(FILES dtd/opensp.dcl DESTINATION "${CMAKE_PREFIX_PATH}/share/libofx/dtd")
install(FILES dtd/ofx160.dtd DESTINATION "${CMAKE_PREFIX_PATH}/share/libofx/dtd")
install(FILES dtd/ofx201.dtd DESTINATION "${CMAKE_PREFIX_PATH}/share/libofx/dtd")
install(FILES dtd/ofc.dtd DESTINATION "${CMAKE_PREFIX_PATH}/share/libofx/dtd")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libofx.pc" DESTINATION "${CMAKE_PREFIX_PATH}/lib/pkgconfig")