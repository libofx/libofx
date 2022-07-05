# SPDX-FileCopyrightText: 2022 Dawid Wróbel <me@dawidwrobel.com>

set(ofxconnect_SRCS
    cmdline.c
    ofxconnect.cpp 
    ofxpartner.cpp 
    nodeparser.cpp
)

include_directories(
    ${LIBXMLPP_INCLUDE_DIRS}
    ${LIBCURL_INCLUDE_DIRS}
)

# required by libxml++
set (CMAKE_CXX_STANDARD 11)

if(MSVC)
    set(ofxconnect_SRCS ${ofxconnect_SRCS} ../lib/messages.cpp ../lib/getopt.c ../lib/getopt1.c)
endif(MSVC)

add_definitions(-DCMDLINE_PARSER_PACKAGE=\"ofxconnect\" -DCMDLINE_PARSER_PACKAGE_NAME=\"ofxconnect\" -DCMDLINE_PARSER_VERSION=\"${VERSION}\")
add_executable(ofxconnect ${ofxconnect_SRCS})
target_link_libraries(ofxconnect ofx CURL::libcurl PkgConfig::LIBXMLPP)
