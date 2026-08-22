# FastPhysics

FastPhysics is a small scientific simulation engine written in modern C++.

The project is used to explore:

- numerical simulation
- modern C++
- computer architecture
- performance engineering
- optimization
- benchmarking

## Build

### Debug

```bash
cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug
```

### Release

```bash
cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release
cmake --build build/release
```