# FastPhysics

FastPhysics is a C++20 N-body gravitational simulation project used to study numerical methods and performance engineering.

## Current features

- Direct gravitational solver with softening.
- Euler and Velocity Verlet integration.
- Array of Structures (AoS) and Structure of Arrays (SoA) particle representations.
- Direct solver variants exploring pairwise interactions, vectorization, manual AVX/FMA, and OpenMP.
- Correctness tests and performance benchmarks.

## Barnes–Hut solver

FastPhysics is developing a Barnes–Hut gravity solver to approximate interactions between distant groups of particles. It organizes particles in an octree and represents sufficiently distant regions by their total mass and center of mass.

The goal is to compare this approach with the optimized direct solvers across different particle distributions and system sizes.

## Build

The manual SIMD implementation requires a compiler and CPU with AVX and FMA support. OpenMP is enabled by default.

### Debug

```bash
cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug -j
```

### Release

```bash
cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release
cmake --build build/release -j
```

## Tests

Run all tests from the project root:

```bash
ctest --test-dir build/debug --output-on-failure
```