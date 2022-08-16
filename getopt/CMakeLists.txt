# SPDX-FileCopyrightText: 2022 Dawid Wróbel <me@dawidwrobel.com>
# SPDX-License-Identifier: GPL-2.0-or-later

include(CheckIncludeFile)
include(CheckSymbolExists)
check_include_file("getopt.h" HAVE_GETOPT_H)
check_symbol_exists("getopt_long" "getopt.h" HAVE_GETOPT_LONG)

if (HAVE_GETOPT_H)
    set(HAVE_GETOPT_H 1 PARENT_SCOPE)
endif ()
if (HAVE_GETOPT_LONG)
    set(HAVE_GETOPT_LONG 1 PARENT_SCOPE)
endif ()

# C library on non-POSIX systems does not come with `getopt` by default.
# Some third-party libraries, however, are commonly used. `vcpkg`, for example, provides `getopt-win32` for Windows,
# based on https://github.com/libimobiledevice-win32/getopt
# Another commonly-used implementation is https://github.com/takamin/win-c
find_library(GETOPT_LIBRARY NAMES getopt DOC "A non-glibc, external getopt library")

if (GETOPT_LIBRARY)
    message(STATUS "Found an external getopt library: ${GETOPT_LIBRARY}")

    if (NOT HAVE_GETOPT_LONG)
        find_path(GETOPT_INCLUDE_DIR NAMES getopt.h)
        if (GETOPT_INCLUDE_DIR)
            message(STATUS "Found getopt includes: ${GETOPT_INCLUDE_DIR}")
        else ()
            message(ERROR "Couldn't locate matching includes for the external library!")
        endif ()
    endif ()

    if (NOT TARGET getopt)
        add_library(getopt UNKNOWN IMPORTED GLOBAL)
        if (GETOPT_INCLUDE_DIR)
            set_target_properties(getopt PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${GETOPT_INCLUDE_DIR}")
        endif ()
        set_property(TARGET getopt APPEND PROPERTY IMPORTED_LOCATION "${GETOPT_LIBRARY}")
    endif ()
else ()
    # If we're not on POSIX system and an external library wasn't found, let's fallback to our copy of the glibc code
    if (NOT HAVE_GETOPT_LONG AND NOT TARGET getopt)
        message(STATUS "A 3rd-party getopt library was not found, so using own copy.")
        add_library(getopt STATIC getopt.c getopt1.c getopt.h)
        set_target_properties(getopt PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}")
    endif ()
endif ()
