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
)

if (WIN32)
    set(ofx_SRCS ${ofx_SRCS} win32.cpp)
    set(ofx_HEADERS ${ofx_HEADERS} win32.hh)
endif ()

add_library(libofx ${ofx_SRCS} ${ofx_HEADERS})
set_target_properties(libofx PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
set_target_properties(libofx PROPERTIES SOVERSION ${LIBOFX_LIBRARY_SONAME})
set_target_properties(libofx PROPERTIES VERSION ${LIBOFX_LIBRARY_VERSION})
set_target_properties(libofx PROPERTIES PREFIX "")

target_link_libraries(libofx OpenSP::OpenSP)
target_include_directories(libofx BEFORE
        PUBLIC $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/inc> $<INSTALL_INTERFACE:include>
        PRIVATE ${CMAKE_BINARY_DIR}
)
target_compile_definitions(libofx PRIVATE IN_LIBOFX=1 MAKEFILE_DTD_PATH="${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/libofx/dtd/")

if (CMAKE_COMPILER_IS_GNUCXX)
    set_target_properties(libofx PROPERTIES CXX_VISIBILITY_PRESET hidden)
endif (CMAKE_COMPILER_IS_GNUCXX)

if (MSVC)
    set_target_properties(libofx PROPERTIES OUTPUT_NAME "ofx")
endif ()

if (Iconv_FOUND)
    target_link_libraries(libofx Iconv::Iconv)
endif ()

install(TARGETS libofx
        EXPORT LibOFXTargets
        LIBRARY
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
