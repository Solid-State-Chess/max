cmake_minimum_required(VERSION 3.20)
include(CMakeDependentOption)

project(
    max
    VERSION 0.2.0
    LANGUAGES C
)

option(MAX_PACKED_MOVE "Enable 2 byte move representation instead of 3 bytes" OFF)
option(MAX_TESTS   "Enable max-tests compilation" ON)
option(MAX_DOC     "Enable Doxygen documentation build" OFF)
option(MAX_ASSERTS "Enable internal self-check assertions for debugging" OFF)
option(MAX_CONSOLE "Enable console formatting functions, mostly for debugging boards" OFF)

if(MAX_TESTS)
    set(MAX_ASSERTS ON)
    set(MAX_CONSOLE ON)
endif()

enable_testing()
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)



file(
    GLOB SRC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*/*.c"
)

add_library(max STATIC ${SRC})

target_include_directories(max PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(max PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/include")
target_compile_definitions(
    max
    PUBLIC
    $<IF:$<STREQUAL:${MAX_TESTS},ON>,MAX_TESTS,>
    $<IF:$<STREQUAL:${MAX_ASSERTS},ON>,MAX_ASSERTS,>
    $<IF:$<STREQUAL:${MAX_CONSOLE},ON>,MAX_CONSOLE,>
    $<IF:$<STREQUAL:${MAX_PACKED_MOVE},ON>,MAX_PACKED_MOVE,>
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
    file(GLOB TEST_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.c")
    add_executable(max-tests ${TEST_SRC})
    target_link_libraries(max-tests PUBLIC max)
    
    add_test(
        NAME run-max-tests
        COMMAND max-tests
    )
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
    
        doxygen_add_docs(
            max-doc
            ${SRC}
            ${INCLUDE_FILES}
            CONFIG_FILE "${DOXYFILE_OUT}"
        )
    endif()
endif()
