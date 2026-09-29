#include "fastphysics/gravity.hpp"
#include "fastphysics/particle_system_soa.hpp"
#include "fastphysics/scenarios.hpp"

#include <omp.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

namespace {

struct BenchmarkResult {
    double best_time_ms{};
    double median_time_ms{};
    double average_time_ms{};
};

// The median is less sensitive to occasional slow runs caused by operating
// system scheduling, background processes, or other transient effects.

// With an odd number of repetitions, the median is simply the middle value
// after sorting the measurements.
double calculate_median(
    std::vector<double> timings
)
{
    std::sort(
        timings.begin(),
        timings.end()
    );

    const std::size_t middle = timings.size() / 2;

    if (timings.size() % 2 == 0) {
        return (timings[middle - 1] + timings[middle]) / 2.0;
    }

    return timings[middle];
}

// AoS and SoA benchmarks use different particle types, so we keep separate
// timing functions instead of forcing all layouts through the same interface.
BenchmarkResult benchmark_reference_aos(
    const std::vector<fastphysics::Particle>& initial_particles,
    std::size_t repetitions
)
{
    auto particles = initial_particles;

    constexpr double gravitational_constant = 1.0;
    constexpr double softening = 0.01;

    // Warm-up run to reduce first-run effects.
    // We excecute the algorith but we dont make it count.
    fastphysics::compute_accelerations(
        particles,
        gravitational_constant,
        softening
    );

    double total_time_ms = 0.0;
    double best_time_ms = 0.0;

    std::vector<double> timings;
    timings.reserve(repetitions);

    for (std::size_t repetition = 0; repetition < repetitions; ++repetition) {

        const auto start = std::chrono::steady_clock::now();

        fastphysics::compute_accelerations(
            particles,
            gravitational_constant,
            softening
        );

        const auto end = std::chrono::steady_clock::now();

        const double elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();

        total_time_ms += elapsed_ms;
        timings.push_back(elapsed_ms);

        if (repetition == 0 || elapsed_ms < best_time_ms) {
            best_time_ms = elapsed_ms;
        }
    }

    return {
        best_time_ms,
        calculate_median(timings),
        total_time_ms / static_cast<double>(repetitions)
    };
}

BenchmarkResult benchmark_pairwise_aos(
    const std::vector<fastphysics::Particle>& initial_particles,
    std::size_t repetitions
)
{
    auto particles = initial_particles;

    constexpr double gravitational_constant = 1.0;
    constexpr double softening = 0.01;

    // Warm-up run.
    fastphysics::compute_accelerations_pairwise(
        particles,
        gravitational_constant,
        softening
    );

    double total_time_ms = 0.0;
    double best_time_ms = 0.0;

    std::vector<double> timings;
    timings.reserve(repetitions);

    for (std::size_t repetition = 0; repetition < repetitions; ++repetition) {

        const auto start = std::chrono::steady_clock::now();

        fastphysics::compute_accelerations_pairwise(
            particles,
            gravitational_constant,
            softening
        );

        const auto end = std::chrono::steady_clock::now();

        const double elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();

        total_time_ms += elapsed_ms;
        timings.push_back(elapsed_ms);

        if (repetition == 0 || elapsed_ms < best_time_ms) {
            best_time_ms = elapsed_ms;
        }
    }

    return {
        best_time_ms,
        calculate_median(timings),
        total_time_ms / static_cast<double>(repetitions)
    };
}

BenchmarkResult benchmark_reference_soa(
    const fastphysics::ParticleSystemSoA& initial_particles,
    std::size_t repetitions
)
{
    auto particles = initial_particles;

    constexpr double gravitational_constant = 1.0;
    constexpr double softening = 0.01;

    // Warm-up run.
    fastphysics::compute_accelerations_soa(
        particles,
        gravitational_constant,
        softening
    );

    double total_time_ms = 0.0;
    double best_time_ms = 0.0;

    std::vector<double> timings;
    timings.reserve(repetitions);

    for (std::size_t repetition = 0; repetition < repetitions; ++repetition) {

        const auto start = std::chrono::steady_clock::now();

        fastphysics::compute_accelerations_soa(
            particles,
            gravitational_constant,
            softening
        );

        const auto end =
            std::chrono::steady_clock::now();

        const double elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();

        total_time_ms += elapsed_ms;
        timings.push_back(elapsed_ms);

        if (repetition == 0 || elapsed_ms < best_time_ms) {
            best_time_ms = elapsed_ms;
        }
    }

    return {
        best_time_ms,
        calculate_median(timings),
        total_time_ms / static_cast<double>(repetitions)
    };
}

BenchmarkResult benchmark_restructured_soa(
    const fastphysics::ParticleSystemSoA& initial_particles,
    std::size_t repetitions
)
{
    auto particles = initial_particles;

    constexpr double gravitational_constant = 1.0;
    constexpr double softening = 0.01;

    // Warm-up run.
    fastphysics::compute_accelerations_soa_restructured(
        particles,
        gravitational_constant,
        softening
    );

    double total_time_ms = 0.0;
    double best_time_ms = 0.0;

    std::vector<double> timings;
    timings.reserve(repetitions);

    for (std::size_t repetition = 0; repetition < repetitions; ++repetition) {

        const auto start = std::chrono::steady_clock::now();

        fastphysics::compute_accelerations_soa_restructured(
            particles,
            gravitational_constant,
            softening
        );

        const auto end = std::chrono::steady_clock::now();

        const double elapsed_ms =
            std::chrono::duration<double, std::milli>(
                end - start
            ).count();

        total_time_ms += elapsed_ms;
        timings.push_back(elapsed_ms);

        if (repetition == 0 || elapsed_ms < best_time_ms) {
            best_time_ms = elapsed_ms;
        }
    }

    return {
        best_time_ms,
        calculate_median(timings),
        total_time_ms / static_cast<double>(repetitions)
    };
}

BenchmarkResult benchmark_avx_soa(
    const fastphysics::ParticleSystemSoA& initial_particles,
    std::size_t repetitions
)
{
    auto particles = initial_particles;

    constexpr double gravitational_constant = 1.0;
    constexpr double softening = 0.01;

    // Warm up the manual AVX implementation before measuring it.
    fastphysics::compute_accelerations_soa_avx(
        particles,
        gravitational_constant,
        softening
    );

    double total_time_ms = 0.0;
    double best_time_ms = 0.0;

    std::vector<double> timings;
    timings.reserve(repetitions);

    for (std::size_t repetition = 0; repetition < repetitions; ++repetition) {

        const auto start = std::chrono::steady_clock::now();

        fastphysics::compute_accelerations_soa_avx(
            particles,
            gravitational_constant,
            softening
        );

        const auto end = std::chrono::steady_clock::now();

        const double elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();

        total_time_ms += elapsed_ms;
        timings.push_back(elapsed_ms);

        if (repetition == 0 || elapsed_ms < best_time_ms) {
            best_time_ms = elapsed_ms;
        }
    }

    return {
        best_time_ms,
        calculate_median(timings),
        total_time_ms / static_cast<double>(repetitions)
    };
}

BenchmarkResult benchmark_avx_openmp_soa(
    const fastphysics::ParticleSystemSoA& initial_particles,
    std::size_t repetitions,
    int thread_count
)
{
    auto particles = initial_particles;

    constexpr double gravitational_constant = 1.0;
    constexpr double softening = 0.01;

    // Disable dynamic thread adjustment so that OpenMP uses the number
    // of threads requested by the benchmark.
    omp_set_dynamic(0);
    omp_set_num_threads(thread_count);

    // Warm up both the AVX calculation and the OpenMP thread team.
    fastphysics::compute_accelerations_soa_avx_openmp(
        particles,
        gravitational_constant,
        softening
    );

    double total_time_ms = 0.0;
    double best_time_ms = 0.0;

    std::vector<double> timings;
    timings.reserve(repetitions);

    for (std::size_t repetition = 0; repetition < repetitions; ++repetition) {

        const auto start = std::chrono::steady_clock::now();

        fastphysics::compute_accelerations_soa_avx_openmp(
            particles,
            gravitational_constant,
            softening
        );

        const auto end = std::chrono::steady_clock::now();

        const double elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();

        total_time_ms += elapsed_ms;
        timings.push_back(elapsed_ms);

        if (repetition == 0 || elapsed_ms < best_time_ms) {
            best_time_ms = elapsed_ms;
        }
    }

    return {
        best_time_ms,
        calculate_median(timings),
        total_time_ms / static_cast<double>(repetitions)
    };
}

BenchmarkResult benchmark_pairwise_soa(
    const fastphysics::ParticleSystemSoA& initial_particles,
    std::size_t repetitions
)
{
    auto particles = initial_particles;

    constexpr double gravitational_constant = 1.0;
    constexpr double softening = 0.01;

    // Warm-up run.
    fastphysics::compute_accelerations_pairwise_soa(
        particles,
        gravitational_constant,
        softening
    );

    double total_time_ms = 0.0;
    double best_time_ms = 0.0;

    std::vector<double> timings;
    timings.reserve(repetitions);

    for (std::size_t repetition = 0; repetition < repetitions; ++repetition) {

        const auto start = std::chrono::steady_clock::now();

        fastphysics::compute_accelerations_pairwise_soa(
            particles,
            gravitational_constant,
            softening
        );

        const auto end = std::chrono::steady_clock::now();

        const double elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();

        total_time_ms += elapsed_ms;
        timings.push_back(elapsed_ms);

        if (repetition == 0 || elapsed_ms < best_time_ms) {
            best_time_ms = elapsed_ms;
        }
    }

    return {
        best_time_ms,
        calculate_median(timings),
        total_time_ms / static_cast<double>(repetitions)
    };
}

// We double N between measurements.
// If T(N) ~ N^p
// then T(2N) / T(N) ~ 2^p
// so p = log2(slowdown) where slowdown = T(2N) / T(N)

// A direct N-body solver should stay close to p = 2 even when constant
// factors change because of pairwise evaluation, memory layout, or
// compiler optimizations.
double calculate_scaling_exponent(double slowdown)
{
    return std::log2(slowdown);
}

void print_result(
    const char* name,
    const BenchmarkResult& result,
    double previous_median_ms
)
{
    std::cout
        << "  " << name << '\n'
        << "    best      : "
        << result.best_time_ms
        << " ms\n"
        << "    median    : "
        << result.median_time_ms
        << " ms\n"
        << "    average   : "
        << result.average_time_ms
        << " ms\n";

    if (previous_median_ms > 0.0) {

        const double slowdown = result.median_time_ms / previous_median_ms;

        const double scaling_exponent = calculate_scaling_exponent(slowdown);

        std::cout
            << "    slowdown  : "
            << slowdown
            << "x\n"
            << "    scaling p : "
            << scaling_exponent
            << '\n';
    }

    std::cout << '\n';
}

void print_parallel_result(
    int thread_count,
    const BenchmarkResult& result,
    double one_thread_time_ms
)
{
    const double speedup = one_thread_time_ms / result.median_time_ms;

    const double efficiency = speedup / static_cast<double>(thread_count);

    std::cout
        << "    "
        << thread_count
        << " thread";

    if (thread_count != 1) {
        std::cout << 's';
    }

    std::cout
        << '\n'
        << "      median     : "
        << result.median_time_ms
        << " ms\n"
        << "      speedup    : "
        << speedup
        << "x\n"
        << "      efficiency : "
        << efficiency * 100.0
        << "%\n\n";
}

}

int main()
{
    constexpr std::array<std::size_t, 5> particle_counts{
        400,
        800,
        1600,
        3200,
        6400
    };

    constexpr std::size_t repetitions = 15;

    // Test powers of two up to 16 threads, while never requesting more
    // threads than the number of processors available to OpenMP.
    const int available_threads = omp_get_num_procs();

    std::vector<int> thread_counts;

    for (int thread_count = 1; thread_count <= available_threads && thread_count <= 16; thread_count *= 2) {
        thread_counts.push_back(thread_count);
    }

    std::cout
        << "\n"
        << "============================================================\n"
        << " FastPhysics - Gravity Optimization Benchmark\n"
        << "============================================================\n\n"
        << "Repetitions       : "
        << repetitions
        << '\n'
        << "OpenMP processors : "
        << available_threads
        << "\n\n"
        << "Reference AoS      : N * (N - 1) interactions\n"
        << "Reference SoA      : N * (N - 1) interactions\n"
        << "Restructured SoA   : N * (N - 1) interactions\n"
        << "Manual AVX SoA     : N * (N - 1) interactions\n"
        << "AVX + OpenMP SoA   : N * (N - 1) interactions\n"
        << "Pairwise AoS       : N * (N - 1) / 2 pairs\n"
        << "Pairwise SoA       : N * (N - 1) / 2 pairs\n\n"
        << "All implementations remain O(N^2).\n"
        << "Speedups and scaling use median execution times.\n\n";

    double previous_reference_aos = 0.0;
    double previous_reference_soa = 0.0;
    double previous_restructured_soa = 0.0;
    double previous_avx_soa = 0.0;
    double previous_pairwise_aos = 0.0;
    double previous_pairwise_soa = 0.0;

    for (const auto particle_count : particle_counts) {

        // Generate one physical system and derive the SoA version from it.
        // Every implementation therefore starts from exactly the same data.
        const auto aos_particles =
            fastphysics::make_benchmark_system(
                particle_count
            );

        const auto soa_particles =
            fastphysics::make_particle_system_soa(
                aos_particles
            );

        const BenchmarkResult reference_aos =
            benchmark_reference_aos(
                aos_particles,
                repetitions
            );

        const BenchmarkResult reference_soa =
            benchmark_reference_soa(
                soa_particles,
                repetitions
            );

        const BenchmarkResult restructured_soa =
            benchmark_restructured_soa(
                soa_particles,
                repetitions
            );

        const BenchmarkResult avx_soa =
            benchmark_avx_soa(
                soa_particles,
                repetitions
            );

        const BenchmarkResult pairwise_aos =
            benchmark_pairwise_aos(
                aos_particles,
                repetitions
            );

        const BenchmarkResult pairwise_soa =
            benchmark_pairwise_soa(
                soa_particles,
                repetitions
            );

        std::cout
            << "============================================================\n"
            << "N = "
            << particle_count
            << "\n"
            << "============================================================\n\n";

        print_result(
            "Reference AoS",
            reference_aos,
            previous_reference_aos
        );

        print_result(
            "Reference SoA",
            reference_soa,
            previous_reference_soa
        );

        print_result(
            "Restructured SoA",
            restructured_soa,
            previous_restructured_soa
        );

        print_result(
            "Manual AVX SoA",
            avx_soa,
            previous_avx_soa
        );

        print_result(
            "Pairwise AoS",
            pairwise_aos,
            previous_pairwise_aos
        );

        print_result(
            "Pairwise SoA",
            pairwise_soa,
            previous_pairwise_soa
        );

        // Memory-layout improvement only.
        const double soa_speedup =
            reference_aos.median_time_ms
            / reference_soa.median_time_ms;

        // Improvement from making the SoA loop easier for GCC to vectorize.
        const double restructuring_speedup =
            reference_soa.median_time_ms
            / restructured_soa.median_time_ms;

        // Total improvement of auto-vectorized SoA over the original solver.
        const double restructured_vs_original =
            reference_aos.median_time_ms
            / restructured_soa.median_time_ms;

        // Compare compiler-generated SIMD against manually written AVX.
        //
        // Greater than 1 means Manual AVX is faster.
        const double manual_avx_vs_auto =
            restructured_soa.median_time_ms
            / avx_soa.median_time_ms;

        // Total improvement of Manual AVX over the original AoS solver.
        const double avx_vs_original =
            reference_aos.median_time_ms
            / avx_soa.median_time_ms;

        // Effect of pairwise evaluation while keeping AoS.
        const double pairwise_speedup =
            reference_aos.median_time_ms
            / pairwise_aos.median_time_ms;

        // Effect of pairwise evaluation while keeping SoA.
        const double pairwise_on_soa_speedup =
            reference_soa.median_time_ms
            / pairwise_soa.median_time_ms;

        // Effect of changing to SoA after pairwise optimization.
        const double soa_on_pairwise_speedup =
            pairwise_aos.median_time_ms
            / pairwise_soa.median_time_ms;

        // Compare the pairwise solver against the auto-vectorized full solver.
        const double pairwise_vs_restructured =
            restructured_soa.median_time_ms
            / pairwise_soa.median_time_ms;

        // Greater than 1 means Manual AVX is faster than Pairwise SoA.
        const double avx_vs_pairwise =
            pairwise_soa.median_time_ms
            / avx_soa.median_time_ms;

        // Total improvement of the pairwise SoA solver over the original.
        const double combined_speedup =
            reference_aos.median_time_ms
            / pairwise_soa.median_time_ms;

        std::cout
            << "  Speedups (median)\n"
            << "    SoA on reference          : "
            << soa_speedup
            << "x\n"
            << "    Restructuring on SoA      : "
            << restructuring_speedup
            << "x\n"
            << "    Restructured vs original  : "
            << restructured_vs_original
            << "x\n"
            << "    Manual AVX vs auto-SIMD   : "
            << manual_avx_vs_auto
            << "x\n"
            << "    Manual AVX vs original    : "
            << avx_vs_original
            << "x\n"
            << "    Pairwise on AoS           : "
            << pairwise_speedup
            << "x\n"
            << "    Pairwise on SoA           : "
            << pairwise_on_soa_speedup
            << "x\n"
            << "    SoA on pairwise           : "
            << soa_on_pairwise_speedup
            << "x\n"
            << "    Pairwise vs restructured  : "
            << pairwise_vs_restructured
            << "x\n"
            << "    Manual AVX vs Pairwise    : "
            << avx_vs_pairwise
            << "x\n"
            << "    Combined vs original      : "
            << combined_speedup
            << "x\n\n";

        // OpenMP scaling
        // The 1-thread OpenMP version is our parallel baseline:
        //     speedup(p) = T(1) / T(p)
        //     efficiency(p) = speedup(p) / p
        // This tells us how well the solver scales as more CPU threads
        // are used.

        std::cout << "  AVX + OpenMP scaling\n\n";

        std::vector<BenchmarkResult> parallel_results;
        parallel_results.reserve(
            thread_counts.size()
        );

        for (const int thread_count : thread_counts) {

            parallel_results.push_back(
                benchmark_avx_openmp_soa(
                    soa_particles,
                    repetitions,
                    thread_count
                )
            );
        }

        const double one_thread_time_ms =
            parallel_results.front().median_time_ms;

        for (std::size_t index = 0; index < thread_counts.size(); ++index) {

            print_parallel_result(
                thread_counts[index],
                parallel_results[index],
                one_thread_time_ms
            );
        }

        // Compare the normal serial AVX implementation against the OpenMP
        // implementation using a single thread. This shows the runtime
        // overhead introduced by entering the OpenMP parallel region.
        const double openmp_one_thread_overhead =
            one_thread_time_ms
            / avx_soa.median_time_ms;

        // Compare the fastest tested OpenMP configuration against serial AVX.
        const BenchmarkResult& max_thread_result =
            parallel_results.back();

        const int max_tested_threads =
            thread_counts.back();

        const double parallel_vs_serial_avx =
            avx_soa.median_time_ms
            / max_thread_result.median_time_ms;

        const double parallel_vs_original =
            reference_aos.median_time_ms
            / max_thread_result.median_time_ms;

        std::cout
            << "  OpenMP summary\n"
            << "    1-thread OpenMP / serial AVX : "
            << openmp_one_thread_overhead
            << "x\n"
            << "    "
            << max_tested_threads
            << "-thread vs serial AVX        : "
            << parallel_vs_serial_avx
            << "x\n"
            << "    "
            << max_tested_threads
            << "-thread vs original AoS      : "
            << parallel_vs_original
            << "x\n\n";

        previous_reference_aos =
            reference_aos.median_time_ms;

        previous_reference_soa =
            reference_soa.median_time_ms;

        previous_restructured_soa =
            restructured_soa.median_time_ms;

        previous_avx_soa =
            avx_soa.median_time_ms;

        previous_pairwise_aos =
            pairwise_aos.median_time_ms;

        previous_pairwise_soa =
            pairwise_soa.median_time_ms;
    }

    return 0;
}