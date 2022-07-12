find_package(OpenSP REQUIRED)
find_package(Iconv)

if (Iconv_FOUND)
    set(HAVE_ICONV 1 PARENT_SCOPE)
endif()

include_directories(
        ../inc
        ${CMAKE_CURRENT_BINARY_DIR}
        ${OpenSP_INCLUDE_DIRS}/OpenSP
)

file(GLOB COEFF_HDRS ../inc/libofx.h *.hh )
file(GLOB SRC_SOURCES *.cpp ${COEFF_HDRS})

add_library(ofx ${SRC_SOURCES})

if(MSVC)
    set_target_properties(ofx PROPERTIES OUTPUT_NAME "libofx")
endif(MSVC)

target_link_libraries(ofx ${OpenSP_LIBRARIES} Iconv::Iconv)