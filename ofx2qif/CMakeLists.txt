# SPDX-FileCopyrightText: 2022 Dawid Wróbel <me@dawidwrobel.com>

option(ENABLE_OFX2QIF "Enable ofx2qif tool" ON)
add_feature_info("ofx2qif" ENABLE_OFX2QIF "Enables OFX file to QIF (Quicken Interchange Format) file converter")

if (ENABLE_OFX2QIF)
    set(ofx2qif_SRCS
        ofx2qif.c
    )

    include_directories(
            ../inc
            ${CMAKE_BINARY_DIR}
    )

    add_executable(ofx2qif ${ofx2qif_SRCS})
    target_link_libraries(ofx2qif ofx)
    install(TARGETS ofx2qif RUNTIME DESTINATION bin)
endif()