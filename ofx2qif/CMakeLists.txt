# SPDX-FileCopyrightText: 2022 Dawid Wróbel <me@dawidwrobel.com>
# SPDX-License-Identifier: GPL-2.0-or-later

set(ofx2qif_SRCS
        ofx2qif.c
)

add_executable(ofx2qif ${ofx2qif_SRCS})
target_link_libraries(ofx2qif libofx)

install(TARGETS ofx2qif RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
