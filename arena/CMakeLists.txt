cmake_minimum_required(VERSION 3.20)

project(
    max-arena
    LANGUAGES C
)


set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2-cmake-modules")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

file(
    GLOB SRC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

add_executable(max-arena ${SRC})
target_include_directories(max-arena PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(max-arena PRIVATE SDL2::Main SDL2::Image)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../" "max")
target_link_libraries(max-arena PRIVATE max)
