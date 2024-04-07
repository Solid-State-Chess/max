cmake_minimum_required(VERSION 3.20)

project(
    max-arena
    LANGUAGES C
)


set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../" "max")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2-cmake-modules")

option(
    VENDORED_SDL2
    "Download and compile vendored SDL2 instead of system libraries"
    OFF
)

file(
    GLOB SRC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

add_executable(
    max-arena
    ${SRC}
)


target_include_directories(max-arena PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_options(max-arena PRIVATE
  $<$<CXX_COMPILER_ID:GCC>:-Wall -Wextra -Wpedantic -Werror>
)

target_link_libraries(max-arena PRIVATE max)

if(VENDORED_SDL2)
    include(FetchContent)
    set(FETCHCONTENT_QUIET FALSE)

    FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.30.2
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        EXCLUDE_FROM_ALL
    )

    FetchContent_Declare(
        SDL2_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG release-2.8.2
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        EXCLUDE_FROM_ALL
    )

    target_compile_definitions(max-arena PRIVATE SDL_IMAGE_VENDORED)

    set(SDL2IMAGE_INSTALL ON)
    set(BUILD_SHARED_LIBS FALSE)
    

    FetchContent_MakeAvailable(SDL2)
    FetchContent_MakeAvailable(SDL2_image)

    target_link_libraries(max-arena PRIVATE SDL2::SDL2-static SDL2_image::SDL2_image-static)
else()
    find_package(
        SDL2 REQUIRED
        CONFIG
    )
    find_package(SDL2_image REQUIRED)

    target_link_libraries(
        max-arena
        PRIVATE
        SDL2::SDL2main
        SDL2::Image
    )
endif()
