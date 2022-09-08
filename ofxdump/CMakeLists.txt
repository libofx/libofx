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

install(TARGETS ofxdump RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

####################
### Add tests

# Just a simple check that we can call the exe
add_test (NAME call-ofxdump COMMAND ofxdump)

# This is the list of sample files that fail
set (SAMPLE_OFX_FILENAMES_FAILING
  failing-ofx-file1.xml
  failing-ofx-file2.xml
  ofx-2.1.1-sect-13.13.ofx
)
# This is the list of all sample files (good and fail)
set (SAMPLE_OFX_FILENAMES
  another-example.xml
  example-listing1.xml
  example-listing2.xml
  ofx-2.1.1-sect-13.12.ofx
  ofx-2.1.1-sect-16.5.3.2.ofx
  ofx-2.1.1-sect-16.5.4.2.ofx
  ofx-2.1.1-sect-16.5.6.2.ofx
  ofx-2.1.1-sect-16.5.6.5.ofx
  ofx-2.1.1-sect-8.5.5.ofx
  ofx_spec160_stmtrs_example.sgml
  ofx_spec201_stmtrs_example.xml
  ${SAMPLE_OFX_FILENAMES_FAILING}
)
# Add a test for each file
foreach (fname ${SAMPLE_OFX_FILENAMES})
  set (_testname "test-${fname}")
  add_test (NAME ${_testname} COMMAND ofxdump "${CMAKE_SOURCE_DIR}/doc/ofx_sample_files/${fname}")
  set_property (TEST ${_testname} PROPERTY ENVIRONMENT "OFX_DTD_PATH=${CMAKE_SOURCE_DIR}/dtd")
endforeach ()

# And for the failing file tests, mark them as failing
foreach (fname ${SAMPLE_OFX_FILENAMES_FAILING})
  set (_testname "test-${fname}")
  set_property (TEST ${_testname} PROPERTY WILL_FAIL true)
endforeach ()
