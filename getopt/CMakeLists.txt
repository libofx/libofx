# SPDX-FileCopyrightText: 2022 Dawid Wróbel <me@dawidwrobel.com>

include(CheckIncludeFile)
include(CheckSymbolExists)
check_include_file("getopt.h" HAVE_GETOPT_H)
check_symbol_exists("getopt_long" "getopt.h" HAVE_GETOPT_LONG)

if (HAVE_GETOPT_H)
    set(HAVE_GETOPT_H 1 PARENT_SCOPE)
endif()
if (HAVE_GETOPT_LONG)
    set(HAVE_GETOPT_LONG 1 PARENT_SCOPE)
endif()

# C library on non-POSIX systems does not come with `getopt` by default.
# Some third-party libraries, however, are commonly used. `vcpkg`, for example, provides `getopt-win32` for Windows,
# based on https://github.com/libimobiledevice-win32/getopt
# Another commonly-used implementation is https://github.com/takamin/win-c
if (NOT HAVE_GETOPT_LONG)
    find_path(GETOPT_INCLUDE_DIR NAMES getopt.h)
    find_library(GETOPT_LIBRARY NAMES getopt DOC "A non-glibc, external getopt library")
endif()

# If the library wasn't found, and we're under MSVC, let's fallback to our copy of the
# https://github.com/libimobiledevice-win32/getopt
if (NOT GETOPT_LIBRARY AND MSVC)
    if (NOT TARGET Getopt::Getopt)
        add_library(Getopt::Getopt STATIC getopt.c getopt.h)
        set_target_properties(Getopt::Getopt PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()
else()
    if (NOT TARGET Getopt::Getopt)
        add_library(Getopt::Getopt UNKNOWN IMPORTED)
        set_target_properties(Getopt::Getopt PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${GETOPT_INCLUDE_DIR}")
        set_property(TARGET Getopt::Getopt APPEND PROPERTY IMPORTED_LOCATION "${GETOPT_LIBRARY}")
    endif ()
endif()