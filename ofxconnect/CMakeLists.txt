# SPDX-FileCopyrightText: 2022 Dawid Wróbel <me@dawidwrobel.com>
# SPDX-License-Identifier: GPL-2.0-or-later

cmake_dependent_option(ENABLE_OFXCONNECT "Enable ofxconnect tool" ON
        "CURL_FOUND;PkgConfig_FOUND;LIBXMLPP_FOUND" OFF)
add_feature_info("ofxconnect" ENABLE_OFXCONNECT "Enables ofxconnect utility which allows to test OFX Direct Connect")

if (ENABLE_OFXCONNECT)
    set(ofxconnect_SRCS
            cmdline.c
            ofxconnect.cpp
            ofxpartner.cpp
            nodeparser.cpp
            )

    # required by libxml++
    set(CMAKE_CXX_STANDARD 11)

    add_executable(ofxconnect ${ofxconnect_SRCS})
    target_link_libraries(ofxconnect ofx CURL::libcurl PkgConfig::LIBXMLPP)
    target_include_directories(ofxconnect PRIVATE ${COMMON_INCLUDE_DIRS})
    target_compile_definitions(ofxconnect PRIVATE
            CMDLINE_PARSER_PACKAGE="ofxconnect"
            CMDLINE_PARSER_PACKAGE_NAME="ofxconnect"
            CMDLINE_PARSER_VERSION="${LIBOFX_VERSION_RELEASE_STRING}"
            )

    if (NOT HAVE_GETOPT_LONG)
        target_link_libraries(ofxdump getopt)
    endif ()

    install(TARGETS ofxconnect RUNTIME DESTINATION bin)
endif ()