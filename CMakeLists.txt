cmake_minimum_required(VERSION 3.20)

project(
    FastPhysics
    VERSION 0.1.0
    LANGUAGES CXX
)

add_executable(
    fastphysics
    src/main.cpp
)

target_compile_features(
    fastphysics
    PRIVATE
    cxx_std_20
)

set_target_properties(
    fastphysics
    PROPERTIES
    CXX_EXTENSIONS OFF
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        fastphysics
        PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()