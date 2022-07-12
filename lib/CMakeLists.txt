file(GLOB COEFF_HDRS ../inc/libofx.h *.hh )
file(GLOB SRC_SOURCES *.cpp ${COEFF_HDRS})

add_library(ofx ${SRC_SOURCES})
target_link_libraries(ofx OpenSP::OpenSP)
# ${OpenSP_INCLUDE_DIRS}/OpenSP explicitly required below despite OpenSP::OpenSP target coming pre-populated with
# OpenSP_INCLUDE_DIRS because the source files refer to its header files directly, not via "OpenSP/headers.h" subfolder
target_include_directories(ofx PRIVATE ${OpenSP_INCLUDE_DIRS}/OpenSP ${COMMON_INCLUDE_DIRS})

if(MSVC)
    set_target_properties(ofx PROPERTIES OUTPUT_NAME "libofx")
endif(MSVC)

if (Iconv_FOUND)
    target_link_libraries(ofx Iconv::Iconv)
endif()
