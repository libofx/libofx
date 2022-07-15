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
    message(STATUS "Likely running a non-POSIX system without a getopt-enabled libc. Checking for 3rd-party implementations...")
    find_path(GETOPT_INCLUDE_DIR NAMES getopt.h)
    find_library(GETOPT_LIBRARY NAMES getopt DOC "A non-glibc, external getopt library")

    if (GETOPT_LIBRARY)
        message(STATUS "Found a 3rd-party getopt library and its includes: ${GETOPT_INCLUDE_DIR}")
        if (NOT TARGET getopt)
            add_library(getopt STATIC IMPORTED)
            set_target_properties(getopt PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${GETOPT_INCLUDE_DIR}")
            set_property(TARGET getopt APPEND PROPERTY IMPORTED_LOCATION "${GETOPT_LIBRARY}")
        endif()
    endif()

    # If the library wasn't found, and we're under MSVC, let's fallback to our copy of the
    # https://github.com/takamin/win-c
    if (NOT GETOPT_LIBRARY)
        if (MSVC)
            message(STATUS "A 3rd-party getopt library was not found, but we're compiling with MSVC, so using own copy.")
            if (NOT TARGET getopt)
                add_library(getopt STATIC getopt.c getopt.h)
                target_include_directories(getopt PUBLIC getopt.h)
                set_target_properties(getopt PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}")
            endif()
        else()
            message(SEND_ERROR "Unable to locate a suitable getopt implementation. Consider disabling tools compilation.")
        endif()
    endif()
endif()
