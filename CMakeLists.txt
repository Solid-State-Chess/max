cmake_minimum_required(VERSION 3.20)
include(CMakeDependentOption)

project(
    max
    VERSION 0.2.0
    LANGUAGES C
)

option(MAX_ZOBRIST_64      "Enable 64-bit zobrist over 32-bit keys" OFF)
option(MAX_TESTS   "Enable max-tests compilation" ON)
option(MAX_PERFTREE_BIN "Enable max-perftree binary for use with the perftree utility" OFF)
option(MAX_DOC     "Enable Doxygen documentation build" OFF)
option(MAX_ENGINE_DIAGNOSTIC "Enable internal engine diagnostic tracking" OFF)
option(MAX_ASSERTS "Enable internal self-check assertions for debugging" OFF)
option(MAX_ASSERTS_SANITY "Enable extensive internal sanity checks for movegen and move make / unmake debugging" OFF)
option(MAX_CONSOLE "Enable console formatting functions, mostly for debugging boards" OFF)

if(MAX_TESTS)
    set(MAX_CONSOLE ON)
endif()

enable_testing()
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)



file(
    GLOB SRC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lib.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/board/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/board/**/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/**/*.c"
)

add_library(max STATIC ${SRC})

target_include_directories(max PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(max PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/include")
target_compile_definitions(
    max
    PUBLIC
    $<$<BOOL:${MAX_TESTS}>:MAX_TESTS>
    $<$<BOOL:${MAX_ASSERTS}>:MAX_ASSERTS>
    $<$<BOOL:${MAX_ASSERTS_SANITY}>:MAX_ASSERTS_SANITY>
    $<$<BOOL:${MAX_CONSOLE}>:MAX_CONSOLE>
    $<$<BOOL:${MAX_ZOBRIST_64}>:MAX_ZOBRIST_64>
    $<$<BOOL:${MAX_PERFTREE_BIN}>:MAX_PERFTREE_BIN>
    $<$<BOOL:${MAX_ENGINE_DIAGNOSTIC}>:MAX_ENGINE_DIAGNOSTIC>
)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
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

if(MAX_TESTS)
    add_executable(max-tests "${CMAKE_CURRENT_SOURCE_DIR}/src/bin/test.c")
    target_link_libraries(max-tests PUBLIC max)
    
    add_test(
        NAME run-max-tests
        COMMAND max-tests
    )
endif()

if(MAX_PERFTREE_BIN)
    add_executable(max-perftree "${CMAKE_CURRENT_SOURCE_DIR}/src/bin/perftree.c")
    target_link_libraries(max-perftree PUBLIC max)
endif()

if(MAX_DOC)
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

        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doc")

        add_custom_target(
            max-doc
            COMMAND Doxygen::doxygen ${DOXYFILE_OUT}
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doc"
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )
    endif()
endif()
