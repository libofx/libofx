# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.
# SPDX-FileCopyrightText: 2022 Dawid Wróbel <me@dawidwrobel.com>
# SPDX-License-Identifier: BSD-3-Clause

#[=======================================================================[.rst:
FindOpenSP
------------

Try to find the OpenSP library.

This will define the following variables:

``OpenSP_FOUND``
    True if (the requested version of) OpenSP is available

``OpenSP_VERSION``
    The version of OpenSP

``OpenSP_INCLUDE_DIRS``
    The include dirs of OpenSP for use with target_include_directories()

``OpenSP_LIBRARIES``
    The OpenSP library for use with target_link_libraries().
    This can be passed to target_link_libraries() instead of
    the ``OpenSP::OpenSP`` target

If ``OpenSP_FOUND`` is TRUE, it will also define the following imported
target:

``OpenSP::OpenSP``
    The OpenSP library

In general we recommend using the imported target, as it is easier to use.
Bear in mind, however, that if the target is in the link interface of an
exported library, it must be made available by the package config file.

The following cache variables may also be set:

``OpenSP_INCLUDE_DIR``
  the OpenSP include directory

``OpenSP_LIBRARY``
  the absolute path of the osp library

#]=======================================================================]

find_path(OpenSP_INCLUDE_DIR
        NAMES opensp/ParserEventGeneratorKit.h OpenSP/ParserEventGeneratorKit.h
        DOC "The OpenSP include directory"
        )

find_library(OpenSP_LIBRARY
        NAMES osp sp133
        DOC "The OpenSP (osp) library"
        )

if (OpenSP_INCLUDE_DIR AND EXISTS "${OpenSP_INCLUDE_DIR}/opensp/config.h")
    file(STRINGS "${OpenSP_INCLUDE_DIR}/opensp/config.h" opensp_version_str REGEX "^#define[\t ]+SP_VERSION[\t ]+\".*\"")

    string(REGEX REPLACE "^.*SP_VERSION[\t ]+\"([^\"]*)\".*$" "\\1" OpenSP_VERSION_STRING "${opensp_version_str}")
    unset(opensp_version_str)
endif ()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenSP
        FOUND_VAR OpenSP_FOUND
        REQUIRED_VARS OpenSP_LIBRARY OpenSP_INCLUDE_DIR
        VERSION_VAR OpenSP_VERSION_STRING
        )

if (OpenSP_FOUND)
    set(OpenSP_LIBRARIES ${OpenSP_LIBRARY})
    set(OpenSP_INCLUDE_DIRS ${OpenSP_INCLUDE_DIR})
    if (NOT TARGET OpenSP::OpenSP)
        add_library(OpenSP::OpenSP UNKNOWN IMPORTED)
        set_target_properties(OpenSP::OpenSP PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${OpenSP_INCLUDE_DIRS}")
        set_property(TARGET OpenSP::OpenSP APPEND PROPERTY IMPORTED_LOCATION "${OpenSP_LIBRARY}")
    endif ()
endif ()

mark_as_advanced(OpenSP_INCLUDE_DIR OpenSP_LIBRARY)

include(FeatureSummary)
set_package_properties(OpenSP PROPERTIES
        URL "http://openjade.sourceforge.net/doc/index.htm"
        DESCRIPTION "An SGML System Conforming to International Standard ISO 8879"
        )
