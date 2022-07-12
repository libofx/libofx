
set(ofx_SRCS
        messages.cpp
        ofx_utilities.cpp
        file_preproc.cpp
        context.cpp
        ofx_preproc.cpp
        ofx_container_generic.cpp
        ofx_container_main.cpp
        ofx_container_security.cpp
        ofx_container_statement.cpp
        ofx_container_account.cpp
        ofx_container_transaction.cpp
        ofx_container_position.cpp
        ofx_containers_misc.cpp
        ofx_request.cpp
        ofx_request_accountinfo.cpp
        ofx_request_statement.cpp
        ofx_sgml.cpp
        ofc_sgml.cpp
        win32.cpp
        )

set(ofx_HEADERS
        messages.hh
        ofx_preproc.hh
        file_preproc.hh
        context.hh
        ofx_sgml.hh
        ofc_sgml.hh
        ofx_aggregate.hh
        ofx_error_msg.hh
        ofx_containers.hh
        ofx_request.hh
        ofx_request_accountinfo.hh
        ofx_request_statement.hh
        ofx_utilities.hh
        tree.hh
        win32.hh
        )

add_library(ofx ${ofx_SRCS} ${ofx_HEADERS})
target_link_libraries(ofx OpenSP::OpenSP)
# ${OpenSP_INCLUDE_DIRS}/OpenSP explicitly required below despite OpenSP::OpenSP target coming pre-populated with
# OpenSP_INCLUDE_DIRS because the source files refer to its header files directly, not via "OpenSP/headers.h" subfolder
target_include_directories(ofx PRIVATE ${OpenSP_INCLUDE_DIRS}/OpenSP ${ofx_HEADERS} ${COMMON_INCLUDE_DIRS})

if(MSVC)
    set_target_properties(ofx PROPERTIES OUTPUT_NAME "libofx")
endif(MSVC)

if (Iconv_FOUND)
    target_link_libraries(ofx Iconv::Iconv)
endif()
