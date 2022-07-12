# ${OpenSP_INCLUDE_DIRS}/OpenSP required here despite OpenSP::OpenSP target coming pre-populated with
# OpenSP_INCLUDE_DIRS because the source files refer to its header files directly, not via "OpenSP/headers.h" subfolder
include_directories(
        ../inc
        ${CMAKE_BINARY_DIR}
        ${OpenSP_INCLUDE_DIRS}/OpenSP
)

file(GLOB COEFF_HDRS ../inc/libofx.h *.hh )
file(GLOB SRC_SOURCES *.cpp ${COEFF_HDRS})

if(MSVC)
    set_target_properties(ofx PROPERTIES OUTPUT_NAME "libofx")
endif(MSVC)

add_library(ofx ${SRC_SOURCES})
target_link_libraries(ofx OpenSP::OpenSP)

if (Iconv_FOUND)
    target_link_libraries(ofx Iconv::Iconv)
endif()
