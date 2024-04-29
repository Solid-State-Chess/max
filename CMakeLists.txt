cmake_minimum_required(VERSION 3.20)

project(
    max
    VERSION 0.2.0
    LANGUAGES C
)

enable_testing()
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

file(
    GLOB SRC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*/*.c"
)

add_library(max STATIC ${SRC})

target_include_directories(max PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(CMAKE_BUILD_TYPE EQUAL "Debug")
    target_compile_definitions(max PRIVATE "MAX_DEBUG")
endif()


include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORT OUTPUT IPO_SUPPORT_ERR)
if(IPO_SUPPORT)
    message(STATUS "IPO / LTO enabled")
    set_property(TARGET max PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    target_compile_options(max PRIVATE -march=native)
else()
    message(STATUS "IPO / LTO not supported: <${IPO_SUPPORT_ERR}>")
endif()

file(GLOB TEST_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.c")
add_executable(max-tests ${TEST_SRC})
target_link_libraries(max-tests PUBLIC max)

add_test(
    NAME run-max-tests
    COMMAND max-tests
)

find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_VERBATIM_VARS )
    
    file(
        GLOB INCLUDE_FILES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/max.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/max/*/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*/*/*.h"
    )

    list(JOIN SRC " " SRC_LIST)
    list(JOIN INCLUDE_FILES " " INCLUDE_LIST)

    set(DOXYFILE_OUT "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.max-doc")

    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Doxyfile.cmake.in"
        ${DOXYFILE_OUT}
    )

    doxygen_add_docs(
        max-doc
        ${SRC}
        ${INCLUDE_FILES}
        CONFIG_FILE "${DOXYFILE_OUT}"
    )
endif()
