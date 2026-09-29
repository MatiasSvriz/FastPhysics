#include "fastphysics/gravity.hpp"
#include "fastphysics/particle_system_soa.hpp"
#include "fastphysics/scenarios.hpp"

#include <omp.h>

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <vector>

namespace {

struct BenchmarkResult {
    double best_time_ms{};
    double median_time_ms{};
    double average_time_ms{};
};

double calculate_median(
    std::vector<double> timings
)
{
    std::sort(
        timings.begin(),
        timings.end()
    );

    const std::size_t middle =
        timings.size() / 2;

    if (timings.size() % 2 == 0) {
        return (
            timings[middle - 1]
            + timings[middle]
        ) / 2.0;
    }

    return timings[middle];
}

BenchmarkResult benchmark_serial_avx(
    const fastphysics::ParticleSystemSoA& initial_particles,
    std::size_t repetitions
)
{
    auto particles = initial_particles;

    constexpr double gravitational_constant = 1.0;
    constexpr double softening = 0.01;

    // Warm-up run.
    fastphysics::compute_accelerations_soa_avx(
        particles,
        gravitational_constant,
        softening
    );

    std::vector<double> timings;
    timings.reserve(repetitions);

    double best_time_ms = 0.0;
    double total_time_ms = 0.0;

    for (std::size_t repetition = 0;
         repetition < repetitions;
         ++repetition) {

        const auto start =
            std::chrono::steady_clock::now();

        fastphysics::compute_accelerations_soa_avx(
            particles,
            gravitational_constant,
            softening
        );

        const auto end =
            std::chrono::steady_clock::now();

        const double elapsed_ms =
            std::chrono::duration<double, std::milli>(
                end - start
            ).count();

        timings.push_back(elapsed_ms);

        total_time_ms +=
            elapsed_ms;

        if (repetition == 0
            || elapsed_ms < best_time_ms) {

            best_time_ms =
                elapsed_ms;
        }
    }

    return {
        best_time_ms,
        calculate_median(timings),
        total_time_ms
            / static_cast<double>(repetitions)
    };
}

BenchmarkResult benchmark_openmp_avx(
    const fastphysics::ParticleSystemSoA& initial_particles,
    std::size_t repetitions,
    int thread_count
)
{
    auto particles = initial_particles;

    constexpr double gravitational_constant = 1.0;
    constexpr double softening = 0.01;

    // Keep the requested thread count fixed.
    omp_set_dynamic(0);
    omp_set_num_threads(thread_count);

    // Warm up the OpenMP thread team before measuring.
    fastphysics::compute_accelerations_soa_avx_openmp(
        particles,
        gravitational_constant,
        softening
    );

    std::vector<double> timings;
    timings.reserve(repetitions);

    double best_time_ms = 0.0;
    double total_time_ms = 0.0;

    for (std::size_t repetition = 0;
         repetition < repetitions;
         ++repetition) {

        const auto start =
            std::chrono::steady_clock::now();

        fastphysics::compute_accelerations_soa_avx_openmp(
            particles,
            gravitational_constant,
            softening
        );

        const auto end =
            std::chrono::steady_clock::now();

        const double elapsed_ms =
            std::chrono::duration<double, std::milli>(
                end - start
            ).count();

        timings.push_back(elapsed_ms);

        total_time_ms +=
            elapsed_ms;

        if (repetition == 0
            || elapsed_ms < best_time_ms) {

            best_time_ms =
                elapsed_ms;
        }
    }

    return {
        best_time_ms,
        calculate_median(timings),
        total_time_ms
            / static_cast<double>(repetitions)
    };
}

}

int main()
{
    constexpr std::size_t particle_count = 6400;
    constexpr std::size_t repetitions = 21;

    const int available_threads =
        omp_get_num_procs();

    const auto aos_particles =
        fastphysics::make_benchmark_system(
            particle_count
        );

    const auto soa_particles =
        fastphysics::make_particle_system_soa(
            aos_particles
        );

    std::cout
        << "\n"
        << "============================================================\n"
        << " FastPhysics - OpenMP Scaling Benchmark\n"
        << "============================================================\n\n"
        << "Particles          : "
        << particle_count
        << '\n'
        << "Repetitions        : "
        << repetitions
        << '\n'
        << "Available threads  : "
        << available_threads
        << "\n\n";

    const BenchmarkResult serial_avx =
        benchmark_serial_avx(
            soa_particles,
            repetitions
        );

    std::cout
        << "Serial AVX median  : "
        << serial_avx.median_time_ms
        << " ms\n\n";

    std::vector<BenchmarkResult> results;

    results.reserve(
        static_cast<std::size_t>(
            available_threads
        )
    );

    // Measure every thread count instead of only powers of two.
    for (int thread_count = 1;
         thread_count <= available_threads;
         ++thread_count) {

        results.push_back(
            benchmark_openmp_avx(
                soa_particles,
                repetitions,
                thread_count
            )
        );
    }

    const double one_thread_time_ms =
        results.front().median_time_ms;

    std::cout
        << std::left
        << std::setw(10)
        << "Threads"
        << std::setw(16)
        << "Median (ms)"
        << std::setw(14)
        << "Speedup"
        << std::setw(16)
        << "Efficiency"
        << '\n';

    std::cout
        << "--------------------------------------------------------\n";

    for (int thread_count = 1;
         thread_count <= available_threads;
         ++thread_count) {

        const BenchmarkResult& result =
            results[
                static_cast<std::size_t>(
                    thread_count - 1
                )
            ];

        const double speedup =
            one_thread_time_ms
            / result.median_time_ms;

        const double efficiency =
            speedup
            / static_cast<double>(
                thread_count
            );

        std::cout
            << std::left
            << std::setw(10)
            << thread_count
            << std::setw(16)
            << result.median_time_ms
            << std::setw(14)
            << speedup
            << std::setw(16)
            << efficiency * 100.0
            << '\n';
    }

    const auto best =
        std::min_element(
            results.begin(),
            results.end(),
            [](
                const BenchmarkResult& first,
                const BenchmarkResult& second
            ) {
                return first.median_time_ms
                    < second.median_time_ms;
            }
        );

    const std::size_t best_index =
        static_cast<std::size_t>(
            std::distance(
                results.begin(),
                best
            )
        );

    const int best_thread_count =
        static_cast<int>(
            best_index + 1
        );

    const double best_parallel_speedup =
        serial_avx.median_time_ms
        / best->median_time_ms;

    std::cout
        << "\n"
        << "Best configuration\n"
        << "  threads              : "
        << best_thread_count
        << '\n'
        << "  median               : "
        << best->median_time_ms
        << " ms\n"
        << "  vs serial AVX        : "
        << best_parallel_speedup
        << "x\n\n";

    return 0;
}