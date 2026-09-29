# OpenMP CPU Parallelism

## Goal

The goal of this stage was to add a second level of parallelism to the gravity solver.

Until now, FastPhysics had focused mainly on improving the execution of a single CPU thread:

```text
AoS
 ↓
SoA
 ↓
Loop restructuring
 ↓
Compiler auto-vectorization
 ↓
Manual AVX
```

Manual AVX exploits **data-level parallelism** by processing four `double` values with one 256-bit SIMD instruction.

OpenMP introduces a different type of parallelism:

```text
Thread-level parallelism
```

Several CPU threads can now calculate the accelerations of different particles at the same time.

The two techniques can therefore be combined:

```text
Multiple CPU threads
        ↓
Each thread
        ↓
Manual AVX
        ↓
4 interactions per SIMD operation
```

This gives FastPhysics two independent levels of CPU parallelism.

---

## Parallelizing the outer particle loop

The manual AVX solver already had a convenient structure:

```cpp
for (std::size_t i = 0; i < particle_count; ++i) {
    // Calculate the complete acceleration of particle i.
}
```

Each iteration calculates the acceleration of one target particle `i`.

The OpenMP version adds:

```cpp
#pragma omp parallel for schedule(static)
```

before this outer loop.

Conceptually:

```text
Thread 0 → particle i = ...
Thread 1 → particle i = ...
Thread 2 → particle i = ...
Thread 3 → particle i = ...
```

Inside each thread, AVX is still used to process four source particles `j` simultaneously.

The numerical algorithm itself does not change.

---

## Why the loop is suitable for OpenMP

For each particle `i`, the solver reads:

```text
position_x
position_y
position_z
mass
```

These arrays are read-only during the force calculation, so several threads can safely read them at the same time.

Each iteration finally writes only to:

```cpp
particles.acceleration_x[i]
particles.acceleration_y[i]
particles.acceleration_z[i]
```

Different iterations use different values of `i`, so two threads do not write to the same acceleration element.

The temporary variables are also created inside each iteration:

```cpp
position_i_x
position_i_y
position_i_z

acceleration_x
acceleration_y
acceleration_z

acceleration_x_vector
acceleration_y_vector
acceleration_z_vector

j
```

Each thread therefore owns its own copy of these variables.

This is very different from the pairwise solver, where one interaction updates both particles `i` and `j`. Parallelizing that implementation directly could cause multiple threads to update the same acceleration value and create a race condition.

---

## Why `schedule(static)` was chosen

Every target particle performs approximately the same amount of work:

$$
N-1
$$

gravitational interactions.

The workload is therefore naturally balanced.

For this reason the OpenMP loop uses:

```cpp
schedule(static)
```

instead of dynamically assigning work while the computation is running.

Static scheduling has very little scheduling overhead and is a good match for this solver.

---

# Measuring parallel scaling

A dedicated benchmark was created:

```text
benchmarks/openmp_scaling.cpp
```

The benchmark uses:

```text
N = 6400
21 repetitions
median execution time
```

and tests every thread count from:

$$
1
$$

to:

$$
20.
$$

Two new metrics are used.

### Parallel speedup

$$
S(p)=\frac{T_1}{T_p}
$$

where:

* \(T_1\) is the execution time using one OpenMP thread.
* \(T_p\) is the execution time using \(p\) threads.

For example:

$$
S(4)=3.7
$$

means that four threads execute the solver approximately 3.7 times faster than one thread.

### Parallel efficiency

$$
E(p)=\frac{S(p)}{p}
$$

Efficiency measures how effectively the available threads are being used.

If:

$$
S(4)=4
$$

then:

$$
E(4)=100\%.
$$

Perfect linear scaling is rarely maintained as the number of threads increases.

---

# Initial scaling experiment

Without explicit CPU affinity, the solver scaled extremely well up to six threads.

The approximate results were:

| Threads |      Median |   Speedup | Efficiency |
| ------: | ----------: | --------: | ---------: |
|       1 |    41.05 ms |     1.00× |       100% |
|       2 |    21.70 ms |     1.89× |      94.6% |
|       3 |    14.52 ms |     2.83× |      94.3% |
|       4 |    11.10 ms |     3.70× |      92.5% |
|       5 |     8.83 ms |     4.65× |      93.0% |
|       6 | **7.85 ms** | **5.23×** |  **87.2%** |

This showed that the algorithm itself parallelizes very well.

However, performance suddenly became worse after six threads:

```text
6 threads →  7.85 ms
7 threads → 12.48 ms
8 threads → 18.52 ms
```

This was the first indication that simply increasing the number of threads was not enough.

---

# CPU topology and WSL2

The benchmark was executed inside WSL2 on an Intel Core i7-13800H.

Linux reported:

```text
20 logical CPUs
10 cores
2 threads per core
Microsoft hypervisor
```

and represented the logical processors as:

```text
CPU 0-1   → Core 0
CPU 2-3   → Core 1
CPU 4-5   → Core 2
...
CPU 18-19 → Core 9
```

An important observation is that this is the topology exposed to Linux through WSL2.

It should therefore not be treated as a perfect description of the underlying physical processor.

There are several scheduling layers involved:

```text
OpenMP
   ↓
Linux scheduler
   ↓
WSL2 / Hyper-V
   ↓
Windows scheduler
   ↓
Physical CPU
```

For performance-sensitive multithreaded code, thread placement can therefore have a large effect.

---

# Thread affinity

Two OpenMP affinity configurations were tested:

```bash
OMP_PLACES=cores
OMP_PROC_BIND=close
```

and:

```bash
OMP_PLACES=cores
OMP_PROC_BIND=spread
```

`OMP_PLACES=cores` tells OpenMP to organize the available logical CPUs into places corresponding to the core topology visible to the runtime.

With the WSL topology, OpenMP reported:

```text
{0:2}
{2:2}
{4:2}
{6:2}
{8:2}
{10:2}
{12:2}
{14:2}
{16:2}
{18:2}
```

Each place therefore contained two logical CPUs.

`close` tries to place threads close to each other.

`spread` tries to distribute them across the available places.

---

# Effect of `spread`

`OMP_PROC_BIND=spread` produced much more stable scaling than the default unbound execution.

For example:

```text
Threads    Median

1          40.87 ms
2          20.74 ms
3          14.62 ms
4          11.13 ms
5           9.00 ms
6           7.53 ms
8           9.90 ms
12          7.69 ms
16          8.76 ms
20          7.36 ms
```

The important result is that thread placement alone changed the 20-thread performance from approximately:

```text
14.8 ms
```

without explicit affinity to approximately:

```text
7.4 ms
```

with `spread`.

No physics, AVX instructions, memory layout, or asymptotic complexity changed.

Only thread placement changed.

This demonstrates that:

> More threads are useful only if those threads are mapped effectively onto the hardware resources.

---

# Maximum performance vs parallel efficiency

The fastest measured configuration was approximately:

```text
20 threads
7.35 ms
5.58× OpenMP speedup
27.9% parallel efficiency
```

However, six threads already achieved:

```text
6 threads
7.48 ms
5.48× OpenMP speedup
91.4% parallel efficiency
```

The difference in execution time is only about:

$$
1.7\%
$$

while the 20-thread version uses more than three times as many threads.

This gives two useful definitions of the optimum.

### Maximum absolute performance

```text
20 threads
≈ 7.35 ms
```

This configuration minimizes execution time in this experiment.

### Efficient operating point

```text
6 threads
≈ 7.48 ms
≈ 91% efficiency
```

This configuration achieves almost the same execution time while using far fewer CPU resources.

This distinction is important in high-performance computing.

The fastest configuration is not necessarily the most efficient use of the machine.

---

# Why scaling stops being linear

The ideal case would be:

$$
S(p)=p.
$$

In practice, increasing the number of threads eventually introduces diminishing returns.

Possible limiting factors include:

```text
shared execution resources
cache contention
memory traffic
SMT / Hyper-Threading
heterogeneous CPU resources
synchronization
thread scheduling
CPU frequency and power limits
```

FastPhysics is particularly demanding because each worker continuously executes computationally expensive operations such as:

```text
AVX arithmetic
FMA
square roots
divisions
memory loads
```

Adding another software thread therefore does not necessarily add an equivalent amount of physical compute capacity.

---

# SIMD and multithreading are different forms of parallelism

One of the main lessons from this stage is the distinction between SIMD and threads.

AVX provides:

```text
Data-level parallelism

one instruction
      ↓
four doubles
```

OpenMP provides:

```text
Thread-level parallelism

several independent workers
      ↓
different particles
```

FastPhysics now combines them:

```text
                    Gravity solver
                         │
            ┌────────────┴────────────┐
            │                         │
         OpenMP                      AVX
            │                         │
   multiple CPU threads      four doubles / instruction
            │                         │
            └────────────┬────────────┘
                         │
                  combined parallelism
```

This is an important step toward understanding CPU high-performance computing.

---

# Performance evolution

The optimization process is no longer simply about reducing the number of mathematical operations.

FastPhysics has progressively explored several different aspects of hardware performance:

```text
Reference AoS
     ↓
better memory layout
     ↓
SoA
     ↓
better loop structure
     ↓
compiler auto-vectorization
     ↓
explicit SIMD control
     ↓
Manual AVX
     ↓
thread-level parallelism
     ↓
OpenMP
     ↓
thread placement
     ↓
OpenMP affinity
```

The algorithm remains a direct N-body solver:

$$
O(N^2).
$$

The large runtime improvements come from executing that same computational complexity more efficiently on the CPU.

---

# Main lessons from Day 18

The most important conclusions are:

1. **SIMD and multithreading solve different problems and can be combined.**

   AVX processes multiple data values per instruction, while OpenMP executes several independent iterations concurrently.

2. **The outer particle loop is naturally parallel.**

   Each iteration calculates one particle's acceleration and writes to a unique output location.

3. **Race conditions depend on the memory access pattern.**

   The full-interaction solver is easy to parallelize, while the pairwise solver is more difficult because interactions update two particles.

4. **Near-linear scaling is possible only while useful independent hardware resources remain available.**

   The solver reached approximately 90% efficiency around six threads.

5. **Logical processor count is not the same as useful compute capacity.**

   A machine exposing 20 logical CPUs does not imply a compute-bound AVX kernel will scale linearly to 20 workers.

6. **Thread affinity can dramatically affect performance.**

   Explicitly distributing threads with `OMP_PROC_BIND=spread` roughly halved the 20-thread runtime compared with the initial unbound experiment.

7. **Maximum speed and maximum efficiency are different optimization goals.**

   Twenty threads gave the smallest measured runtime, but six threads achieved almost the same performance with much higher parallel efficiency.

8. **Benchmarks are necessary to test hardware assumptions.**

   The first results suggested that six threads might be a hard hardware limit. Affinity experiments showed that the real explanation was more complicated and that thread placement was a major factor.

---

# Current result

For the `N = 6400` workload, the final OpenMP experiment produced approximately:

```text
Serial Manual AVX
≈ 41 ms

AVX + OpenMP, 6 threads
≈ 7.48 ms

AVX + OpenMP, 20 threads with spread affinity
≈ 7.35 ms
```

Therefore OpenMP gives approximately:

$$
\boxed{5.5\times}
$$

additional acceleration over the already vectorized manual AVX implementation.

The most important result, however, is not the final number.

The experiment demonstrates that CPU performance depends on several interacting layers:

$$
\boxed{
\text{algorithm}
+
\text{memory layout}
+
\text{SIMD}
+
\text{threads}
+
\text{scheduling}
+
\text{hardware topology}
}
$$

Optimizing numerical software means understanding all of them.
