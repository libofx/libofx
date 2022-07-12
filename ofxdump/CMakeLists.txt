# SPDX-FileCopyrightText: 2009 Cristian Oneț <onet.cristian@gmail.com>
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

    if(MSVC)
        set(ofxdump_SRCS ${ofxdump_SRCS} ../lib/messages.cpp ../lib/getopt.c ../lib/getopt1.c)
    endif(MSVC)

    add_definitions(-DCMDLINE_PARSER_PACKAGE=\"ofxdump\" -DCMDLINE_PARSER_PACKAGE_NAME=\"ofxdump\" -DCMDLINE_PARSER_VERSION=\"${VERSION}\")
    add_executable(ofxdump ${ofxdump_SRCS})
    target_link_libraries(ofxdump ofx)
endif()