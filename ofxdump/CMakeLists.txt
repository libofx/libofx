# SPDX-FileCopyrightText: 2022 Dawid Wróbel <me@dawidwrobel.com>

option(ENABLE_OFXDUMP "Enable ofxdump tool" ON)
add_feature_info("ofxdump" ENABLE_OFXDUMP "Enables ofxconnect utility which prints, in human readable form, everything the library understands about a file")

if (ENABLE_OFXDUMP)
    set(ofxdump_SRCS
        cmdline.c
        ofxdump.cpp
    )

     include_directories(
            ../inc
            ${CMAKE_BINARY_DIR}
    )

    add_definitions(-DCMDLINE_PARSER_PACKAGE=\"ofxdump\" -DCMDLINE_PARSER_PACKAGE_NAME=\"ofxdump\" -DCMDLINE_PARSER_VERSION=\"${VERSION}\")
    add_executable(ofxdump ${ofxdump_SRCS})
    target_link_libraries(ofxdump ofx)

    if (NOT HAVE_GETOPT_LONG)
        include_directories(${GETOPT_INCLUDE_DIR})
        target_link_libraries(ofxdump Getopt::Getopt)
    endif()
endif()