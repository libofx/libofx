# SPDX-FileCopyrightText: 2009 Cristian Oneț <onet.cristian@gmail.com>
# SPDX-FileCopyrightText: 2022 Dawid Wróbel <me@dawidwrobel.com>

######### add a utility function so that we can test ofx files ##########
set(ofxdump_SRCS
    cmdline.c
    ofxdump.cpp 
)

if(MSVC)
    set(ofxdump_SRCS ${ofxdump_SRCS} ../lib/messages.cpp ../lib/getopt.c ../lib/getopt1.c)
endif(MSVC)

add_definitions(-DCMDLINE_PARSER_PACKAGE=\"ofxdump\" -DCMDLINE_PARSER_PACKAGE_NAME=\"ofxdump\" -DCMDLINE_PARSER_VERSION=\"${VERSION}\")
add_executable(ofxdump ${ofxdump_SRCS})
target_link_libraries(ofxdump ofx)
