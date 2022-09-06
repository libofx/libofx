# SPDX-FileCopyrightText: 2022 Dawid Wróbel <me@dawidwrobel.com>
# SPDX-License-Identifier: GPL-2.0-or-later

set(ofxconnect_SRCS
        cmdline.c
        ofxconnect.cpp
        ofxpartner.cpp
        nodeparser.cpp
)

# required by libxml++
if (CMAKE_CXX_STANDARD LESS 11)
    set(CMAKE_CXX_STANDARD 11)
endif()

add_executable(ofxconnect ${ofxconnect_SRCS})
target_link_libraries(ofxconnect ofx CURL::libcurl PkgConfig::LIBXMLPP)
target_include_directories(ofxconnect BEFORE PRIVATE ${COMMON_INCLUDE_DIRS})
target_compile_definitions(ofxconnect PRIVATE
        CMDLINE_PARSER_PACKAGE="ofxconnect"
        CMDLINE_PARSER_PACKAGE_NAME="ofxconnect"
        CMDLINE_PARSER_VERSION="${LIBOFX_VERSION_RELEASE_STRING}"
)

if (TARGET getopt)
    target_link_libraries(ofxconnect getopt)
endif ()

install(TARGETS ofxconnect RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
