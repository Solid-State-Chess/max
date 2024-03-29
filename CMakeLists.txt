cmake_minimum_required(VERSION 3.20)

project(
    max
    LANGUAGES C
)

enable_testing()
list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

file(GLOB SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

add_library(max STATIC ${SRC})
target_include_directories(max PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(CMAKE_BUILD_TYPE EQUAL "Debug")
    target_compile_definitions(max PRIVATE "MAX_DEBUG")
endif()

file(GLOB TEST_SRC CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.c")
add_executable(max-tests ${TEST_SRC})
target_link_libraries(max-tests PUBLIC max)

add_test(
    NAME run-max-tests
    COMMAND max-tests
)
