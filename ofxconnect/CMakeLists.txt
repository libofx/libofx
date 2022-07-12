# SPDX-FileCopyrightText: 2022 Dawid Wróbel <me@dawidwrobel.com>

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

    add_definitions(-DCMDLINE_PARSER_PACKAGE=\"ofxconnect\" -DCMDLINE_PARSER_PACKAGE_NAME=\"ofxconnect\" -DCMDLINE_PARSER_VERSION=\"${VERSION}\")
    add_executable(ofxconnect ${ofxconnect_SRCS})
    target_link_libraries(ofxconnect ofx CURL::libcurl PkgConfig::LIBXMLPP)

    if (NOT HAVE_GETOPT_LONG)
        target_link_libraries(ofxdump getopt)
    endif()

    install(TARGETS ofxconnect RUNTIME DESTINATION bin)
endif()