cmake_minimum_required(VERSION 3.20)

project(
    FastPhysics
    VERSION 0.1.0
    LANGUAGES CXX
)

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------

option(
    FASTPHYSICS_ENABLE_SANITIZERS
    "Enable AddressSanitizer and UndefinedBehaviorSanitizer"
    OFF
)

option(
    FASTPHYSICS_ENABLE_NATIVE_OPTIMIZATIONS
    "Enable CPU-specific optimizations using -march=native"
    OFF
)

option(
    FASTPHYSICS_ENABLE_VECTORIZATION_REPORT
    "Enable compiler vectorization diagnostics"
    OFF
)

option(
    FASTPHYSICS_ENABLE_OPENMP
    "Enable OpenMP multithreading"
    ON
)

# ---------------------------------------------------------------------------
# Core library
# ---------------------------------------------------------------------------

add_library(
    fastphysics_core
    src/vec3.cpp
    src/gravity.cpp
    src/integrator.cpp
    src/simulation.cpp
    src/scenarios.cpp
    src/csv_writer.cpp
    src/energy.cpp
    src/convergence.cpp
    src/particle_system_soa.cpp
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

# ---------------------------------------------------------------------------
# Manual SIMD support
# ---------------------------------------------------------------------------

# gravity.cpp contains explicit AVX and FMA intrinsics.
#
# -mavx enables 256-bit AVX floating-point instructions.
# -mfma enables fused multiply-add instructions such as _mm256_fmadd_pd().
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

    target_compile_options(
        fastphysics_core
        PRIVATE
        -mavx
        -mfma
    )

endif()

# ---------------------------------------------------------------------------
# OpenMP
# ---------------------------------------------------------------------------

# OpenMP is used by the multithreaded gravity solver.
#
# Linking it PUBLICly through fastphysics_core propagates the OpenMP
# requirement to executables and tests that use the core library.
if(FASTPHYSICS_ENABLE_OPENMP)

    find_package(
        OpenMP
        REQUIRED
    )

    target_link_libraries(
        fastphysics_core
        PUBLIC
        OpenMP::OpenMP_CXX
    )

endif()

# ---------------------------------------------------------------------------
# Main executable
# ---------------------------------------------------------------------------

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

# ---------------------------------------------------------------------------
# Benchmarks
# ---------------------------------------------------------------------------

# Gravity optimization benchmark

add_executable(
    gravity_benchmark
    benchmarks/gravity_benchmark.cpp
)

target_link_libraries(
    gravity_benchmark
    PRIVATE
    fastphysics_core
)

set_target_properties(
    gravity_benchmark
    PROPERTIES
    CXX_EXTENSIONS OFF
)

# Memory layout benchmark

add_executable(
    memory_layout
    benchmarks/memory_layout.cpp
)

target_link_libraries(
    memory_layout
    PRIVATE
    fastphysics_core
)

set_target_properties(
    memory_layout
    PROPERTIES
    CXX_EXTENSIONS OFF
)

# OpenMP scaling benchmark
#
# This benchmark uses the OpenMP runtime directly to control the number
# of threads, so it is only built when OpenMP support is enabled.
if(FASTPHYSICS_ENABLE_OPENMP)

    add_executable(
        openmp_scaling
        benchmarks/openmp_scaling.cpp
    )

    target_link_libraries(
        openmp_scaling
        PRIVATE
        fastphysics_core
    )

    set_target_properties(
        openmp_scaling
        PROPERTIES
        CXX_EXTENSIONS OFF
    )

endif()

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------

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

# Particle System SoA tests

add_executable(
    particle_system_soa_tests
    tests/particle_system_soa_tests.cpp
)

target_link_libraries(
    particle_system_soa_tests
    PRIVATE
    fastphysics_core
)

set_target_properties(
    particle_system_soa_tests
    PROPERTIES
    CXX_EXTENSIONS OFF
)

add_test(
    NAME particle_system_soa_tests
    COMMAND particle_system_soa_tests
)

# Gravity tests

add_executable(
    gravity_tests
    tests/gravity_tests.cpp
)

target_link_libraries(
    gravity_tests
    PRIVATE
    fastphysics_core
)

set_target_properties(
    gravity_tests
    PROPERTIES
    CXX_EXTENSIONS OFF
)

add_test(
    NAME gravity_tests
    COMMAND gravity_tests
)

# Integrator tests

add_executable(
    integrator_tests
    tests/integrator_tests.cpp
)

target_link_libraries(
    integrator_tests
    PRIVATE
    fastphysics_core
)

set_target_properties(
    integrator_tests
    PROPERTIES
    CXX_EXTENSIONS OFF
)

add_test(
    NAME integrator_tests
    COMMAND integrator_tests
)

# Simulation tests

add_executable(
    simulation_tests
    tests/simulation_tests.cpp
)

target_link_libraries(
    simulation_tests
    PRIVATE
    fastphysics_core
)

set_target_properties(
    simulation_tests
    PROPERTIES
    CXX_EXTENSIONS OFF
)

add_test(
    NAME simulation_tests
    COMMAND simulation_tests
)

# Scenario tests

add_executable(
    scenario_tests
    tests/scenario_tests.cpp
)

target_link_libraries(
    scenario_tests
    PRIVATE
    fastphysics_core
)

set_target_properties(
    scenario_tests
    PROPERTIES
    CXX_EXTENSIONS OFF
)

add_test(
    NAME scenario_tests
    COMMAND scenario_tests
)

# CSV writer tests

add_executable(
    csv_writer_tests
    tests/csv_writer_tests.cpp
)

target_link_libraries(
    csv_writer_tests
    PRIVATE
    fastphysics_core
)

set_target_properties(
    csv_writer_tests
    PROPERTIES
    CXX_EXTENSIONS OFF
)

add_test(
    NAME csv_writer_tests
    COMMAND csv_writer_tests
)

# Energy tests

add_executable(
    energy_tests
    tests/energy_tests.cpp
)

target_link_libraries(
    energy_tests
    PRIVATE
    fastphysics_core
)

set_target_properties(
    energy_tests
    PROPERTIES
    CXX_EXTENSIONS OFF
)

add_test(
    NAME energy_tests
    COMMAND energy_tests
)

# Convergence tests

add_executable(
    convergence_tests
    tests/convergence_tests.cpp
)

target_link_libraries(
    convergence_tests
    PRIVATE
    fastphysics_core
)

set_target_properties(
    convergence_tests
    PROPERTIES
    CXX_EXTENSIONS OFF
)

add_test(
    NAME convergence_tests
    COMMAND convergence_tests
)

# ---------------------------------------------------------------------------
# Compiler warnings
# ---------------------------------------------------------------------------

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
        gravity_benchmark
        PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

    target_compile_options(
        memory_layout
        PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

    if(FASTPHYSICS_ENABLE_OPENMP)

        target_compile_options(
            openmp_scaling
            PRIVATE
            -Wall
            -Wextra
            -Wpedantic
        )

    endif()

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

    target_compile_options(
        particle_system_soa_tests
        PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

    target_compile_options(
        gravity_tests
        PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

    target_compile_options(
        integrator_tests
        PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

    target_compile_options(
        simulation_tests
        PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

    target_compile_options(
        scenario_tests
        PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

    target_compile_options(
        csv_writer_tests
        PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

    target_compile_options(
        energy_tests
        PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

    target_compile_options(
        convergence_tests
        PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

endif()

# ---------------------------------------------------------------------------
# Native CPU optimizations
# ---------------------------------------------------------------------------

if(
    FASTPHYSICS_ENABLE_NATIVE_OPTIMIZATIONS
    AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang"
)

    target_compile_options(
        fastphysics_core
        PRIVATE
        -march=native
    )

    target_compile_options(
        gravity_benchmark
        PRIVATE
        -march=native
    )

    if(FASTPHYSICS_ENABLE_OPENMP)

        target_compile_options(
            openmp_scaling
            PRIVATE
            -march=native
        )

    endif()

endif()

# ---------------------------------------------------------------------------
# Vectorization reports
# ---------------------------------------------------------------------------

if(FASTPHYSICS_ENABLE_VECTORIZATION_REPORT)

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

        target_compile_options(
            fastphysics_core
            PRIVATE
            -fopt-info-vec
            -fopt-info-vec-missed
        )

    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")

        target_compile_options(
            fastphysics_core
            PRIVATE
            -Rpass=loop-vectorize
            -Rpass-missed=loop-vectorize
            -Rpass-analysis=loop-vectorize
        )

    endif()

endif()

# ---------------------------------------------------------------------------
# Sanitizers
# ---------------------------------------------------------------------------

if(
    FASTPHYSICS_ENABLE_SANITIZERS
    AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang"
)

    set(
        FASTPHYSICS_SANITIZER_FLAGS
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    )

    # Core library

    target_compile_options(
        fastphysics_core
        PRIVATE
        ${FASTPHYSICS_SANITIZER_FLAGS}
    )

    # Main executable

    target_compile_options(
        fastphysics
        PRIVATE
        ${FASTPHYSICS_SANITIZER_FLAGS}
    )

    target_link_options(
        fastphysics
        PRIVATE
        ${FASTPHYSICS_SANITIZER_FLAGS}
    )

    # Gravity benchmark

    target_compile_options(
        gravity_benchmark
        PRIVATE
        ${FASTPHYSICS_SANITIZER_FLAGS}
    )

    target_link_options(
        gravity_benchmark
        PRIVATE
        ${FASTPHYSICS_SANITIZER_FLAGS}
    )

    # Memory layout benchmark

    target_compile_options(
        memory_layout
        PRIVATE
        ${FASTPHYSICS_SANITIZER_FLAGS}
    )

    target_link_options(
        memory_layout
        PRIVATE
        ${FASTPHYSICS_SANITIZER_FLAGS}
    )

    # OpenMP scaling benchmark

    if(FASTPHYSICS_ENABLE_OPENMP)

        target_compile_options(
            openmp_scaling
            PRIVATE
            ${FASTPHYSICS_SANITIZER_FLAGS}
        )

        target_link_options(
            openmp_scaling
            PRIVATE
            ${FASTPHYSICS_SANITIZER_FLAGS}
        )

    endif()

    # Vec3 tests

    target_compile_options(
        fastphysics_tests
        PRIVATE
        ${FASTPHYSICS_SANITIZER_FLAGS}
    )

    target_link_options(
        fastphysics_tests
        PRIVATE
        ${FASTPHYSICS_SANITIZER_FLAGS}
    )

    # Particle tests

    target_compile_options(
        particle_tests
        PRIVATE
        ${FASTPHYSICS_SANITIZER_FLAGS}
    )

    target_link_options(
        particle_tests
        PRIVATE
        ${FASTPHYSICS_SANITIZER_FLAGS}
    )

    # Particle System SoA tests

    target_compile_options(
        particle_system_soa_tests
        PRIVATE
        ${FASTPHYSICS_SANITIZER_FLAGS}
    )

    target_link_options(
        particle_system_soa_tests
        PRIVATE
        ${FASTPHYSICS_SANITIZER_FLAGS}
    )

    # Gravity tests

    target_compile_options(
        gravity_tests
        PRIVATE
        ${FASTPHYSICS_SANITIZER_FLAGS}
    )

    target_link_options(
        gravity_tests
        PRIVATE
        ${FASTPHYSICS_SANITIZER_FLAGS}
    )

    # Integrator tests

    target_compile_options(
        integrator_tests
        PRIVATE
        ${FASTPHYSICS_SANITIZER_FLAGS}
    )

    target_link_options(
        integrator_tests
        PRIVATE
        ${FASTPHYSICS_SANITIZER_FLAGS}
    )

    # Simulation tests

    target_compile_options(
        simulation_tests
        PRIVATE
        ${FASTPHYSICS_SANITIZER_FLAGS}
    )

    target_link_options(
        simulation_tests
        PRIVATE
        ${FASTPHYSICS_SANITIZER_FLAGS}
    )

    # Scenario tests

    target_compile_options(
        scenario_tests
        PRIVATE
        ${FASTPHYSICS_SANITIZER_FLAGS}
    )

    target_link_options(
        scenario_tests
        PRIVATE
        ${FASTPHYSICS_SANITIZER_FLAGS}
    )

    # CSV writer tests

    target_compile_options(
        csv_writer_tests
        PRIVATE
        ${FASTPHYSICS_SANITIZER_FLAGS}
    )

    target_link_options(
        csv_writer_tests
        PRIVATE
        ${FASTPHYSICS_SANITIZER_FLAGS}
    )

    # Energy tests

    target_compile_options(
        energy_tests
        PRIVATE
        ${FASTPHYSICS_SANITIZER_FLAGS}
    )

    target_link_options(
        energy_tests
        PRIVATE
        ${FASTPHYSICS_SANITIZER_FLAGS}
    )

    # Convergence tests

    target_compile_options(
        convergence_tests
        PRIVATE
        ${FASTPHYSICS_SANITIZER_FLAGS}
    )

    target_link_options(
        convergence_tests
        PRIVATE
        ${FASTPHYSICS_SANITIZER_FLAGS}
    )

endif()