# SPDX-FileCopyrightText: 2022 Dawid Wróbel <me@dawidwrobel.com>

find_package(CURL)

if (CURL_FOUND)
    set(HAVE_LIBCURL 1 PARENT_SCOPE)
    find_package(PkgConfig)

    if(PkgConfig_FOUND)
        pkg_check_modules(LIBXMLPP IMPORTED_TARGET libxml++-2.6>=2.6)
    endif()
endif()

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

    include_directories(
        ../inc
        ${CMAKE_BINARY_DIR}
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
endif()