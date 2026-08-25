cmake_minimum_required(VERSION 3.20)

project(
    FastPhysics
    VERSION 0.1.0
    LANGUAGES CXX
)

# Core library

add_library(
    fastphysics_core
    src/vec3.cpp
)

target_include_directories(
    fastphysics_core
    PUBLIC
    include
)

target_compile_features(
    fastphysics_core
    PUBLIC
    cxx_std_20
)

set_target_properties(
    fastphysics_core
    PROPERTIES
    CXX_EXTENSIONS OFF
)

# Main executable

add_executable(
    fastphysics
    src/main.cpp
)

target_link_libraries(
    fastphysics
    PRIVATE
    fastphysics_core
)

set_target_properties(
    fastphysics
    PROPERTIES
    CXX_EXTENSIONS OFF
)

# Tests

enable_testing()

# Vec3 tests

add_executable(
    fastphysics_tests
    tests/vec3_tests.cpp
)

target_link_libraries(
    fastphysics_tests
    PRIVATE
    fastphysics_core
)

set_target_properties(
    fastphysics_tests
    PROPERTIES
    CXX_EXTENSIONS OFF
)

add_test(
    NAME vec3_tests
    COMMAND fastphysics_tests
)

# Particle tests

add_executable(
    particle_tests
    tests/particle_tests.cpp
)

target_link_libraries(
    particle_tests
    PRIVATE
    fastphysics_core
)

set_target_properties(
    particle_tests
    PROPERTIES
    CXX_EXTENSIONS OFF
)

add_test(
    NAME particle_tests
    COMMAND particle_tests
)

# Compiler warnings

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        fastphysics_core
        PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

    target_compile_options(
        fastphysics
        PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

    target_compile_options(
        fastphysics_tests
        PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

    target_compile_options(
        particle_tests
        PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()