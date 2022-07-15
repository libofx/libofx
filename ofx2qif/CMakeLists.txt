# SPDX-FileCopyrightText: 2022 Dawid Wróbel <me@dawidwrobel.com>
# SPDX-License-Identifier: GPL-2.0-or-later

option(ENABLE_OFX2QIF "Enable ofx2qif tool" ON)
add_feature_info("ofx2qif" ENABLE_OFX2QIF "Enables OFX file to QIF (Quicken Interchange Format) file converter")

if (ENABLE_OFX2QIF)
    set(ofx2qif_SRCS
        ofx2qif.c
    )

    add_executable(ofx2qif ${ofx2qif_SRCS})
    target_link_libraries(ofx2qif ofx)
    target_include_directories(ofx2qif PRIVATE ${COMMON_INCLUDE_DIRS})

    install(TARGETS ofx2qif RUNTIME DESTINATION bin)
endif()