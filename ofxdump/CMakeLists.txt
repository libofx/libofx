# SPDX-FileCopyrightText: 2022 Dawid Wróbel <me@dawidwrobel.com>

option(ENABLE_OFXDUMP "Enable ofxdump tool" ON)
add_feature_info("ofxdump" ENABLE_OFXDUMP "Enables ofxconnect utility which prints, in human readable form, everything the library understands about a file")

if (ENABLE_OFXDUMP)
    set(ofxdump_SRCS
            cmdline.c
            ofxdump.cpp
            )

    add_executable(ofxdump ${ofxdump_SRCS})
    target_link_libraries(ofxdump ofx)
    target_include_directories(ofxdump PRIVATE ${COMMON_INCLUDE_DIRS})
    target_compile_definitions(ofxdump PRIVATE CMDLINE_PARSER_PACKAGE="ofxdump"
            CMDLINE_PARSER_PACKAGE_NAME="ofxdump"
            CMDLINE_PARSER_VERSION="${LIBOFX_VERSION_RELEASE_STRING}"
            )

    if (NOT HAVE_GETOPT_LONG)
        target_link_libraries(ofxdump getopt)
    endif ()

    install(TARGETS ofxdump RUNTIME DESTINATION bin)
endif ()