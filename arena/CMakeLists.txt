cmake_minimum_required(VERSION 3.20)

project(
    max-arena
    LANGUAGES C
)


set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)

file(
    GLOB SRC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

add_executable(max-arena ${SRC})
target_include_directories(max-arena PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(max-arena PRIVATE SDL2::SDL2 SDL2::SDL2main)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../" "max")
target_link_libraries(max-arena PRIVATE max)
