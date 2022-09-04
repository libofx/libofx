# SPDX-FileCopyrightText: 2022 Dawid Wróbel <me@dawidwrobel.com>
# SPDX-License-Identifier: GPL-2.0-or-later

set(ofxdump_SRCS
        cmdline.c
        ofxdump.cpp
)

add_executable(ofxdump ${ofxdump_SRCS})
target_link_libraries(ofxdump ofx)
target_include_directories(ofxdump BEFORE PRIVATE ${COMMON_INCLUDE_DIRS})
target_compile_definitions(ofxdump PRIVATE CMDLINE_PARSER_PACKAGE="ofxdump"
        CMDLINE_PARSER_PACKAGE_NAME="ofxdump"
        CMDLINE_PARSER_VERSION="${LIBOFX_VERSION_RELEASE_STRING}"
)

if (TARGET getopt)
    target_link_libraries(ofxdump getopt)
endif ()

install(TARGETS ofxdump RUNTIME DESTINATION bin)
