# SPDX-FileCopyrightText: 2022 Dawid Wróbel <me@dawidwrobel.com>

set(ofx2qif_SRCS
    ofx2qif.c
)

add_executable(ofx2qif ${ofx2qif_SRCS})
target_link_libraries(ofx2qif ofx)
