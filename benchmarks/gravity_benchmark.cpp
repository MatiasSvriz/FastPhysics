#include "fastphysics/gravity.hpp"
#include "fastphysics/particle_system_soa.hpp"
#include "fastphysics/scenarios.hpp"

#include <array>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

namespace {

struct BenchmarkResult {
    double best_time_ms{};
    double average_time_ms{};
};

// AoS and SoA benchmarks use different particle types, so we keep two small
// timing functions instead of forcing both layouts through the same interface.
//
// This keeps the benchmark simple and makes it clear what is actually being
// measured in each case.
BenchmarkResult benchmark_reference_aos(
    const std::vector<fastphysics::Particle>& initial_particles,
    std::size_t repetitions
)
{
    auto particles = initial_particles;

    constexpr double gravitational_constant = 1.0;
    constexpr double softening = 0.01;

    // Warm-up run to reduce first-run effects.
    fastphysics::compute_accelerations(
        particles,
        gravitational_constant,
        softening
    );

    double total_time_ms = 0.0;
    double best_time_ms = 0.0;

    for (std::size_t repetition = 0;
         repetition < repetitions;
         ++repetition) {

        const auto start =
            std::chrono::steady_clock::now();

        fastphysics::compute_accelerations(
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

        total_time_ms += elapsed_ms;

        if (repetition == 0 || elapsed_ms < best_time_ms) {
            best_time_ms = elapsed_ms;
        }
    }

    return {
        best_time_ms,
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

    fastphysics::compute_accelerations_pairwise(
        particles,
        gravitational_constant,
        softening
    );

    double total_time_ms = 0.0;
    double best_time_ms = 0.0;

    for (std::size_t repetition = 0;
         repetition < repetitions;
         ++repetition) {

        const auto start =
            std::chrono::steady_clock::now();

        fastphysics::compute_accelerations_pairwise(
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

        total_time_ms += elapsed_ms;

        if (repetition == 0 || elapsed_ms < best_time_ms) {
            best_time_ms = elapsed_ms;
        }
    }

    return {
        best_time_ms,
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

    fastphysics::compute_accelerations_soa(
        particles,
        gravitational_constant,
        softening
    );

    double total_time_ms = 0.0;
    double best_time_ms = 0.0;

    for (std::size_t repetition = 0;
         repetition < repetitions;
         ++repetition) {

        const auto start =
            std::chrono::steady_clock::now();

        fastphysics::compute_accelerations_soa(
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

        total_time_ms += elapsed_ms;

        if (repetition == 0 || elapsed_ms < best_time_ms) {
            best_time_ms = elapsed_ms;
        }
    }

    return {
        best_time_ms,
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

    fastphysics::compute_accelerations_pairwise_soa(
        particles,
        gravitational_constant,
        softening
    );

    double total_time_ms = 0.0;
    double best_time_ms = 0.0;

    for (std::size_t repetition = 0;
         repetition < repetitions;
         ++repetition) {

        const auto start =
            std::chrono::steady_clock::now();

        fastphysics::compute_accelerations_pairwise_soa(
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

        total_time_ms += elapsed_ms;

        if (repetition == 0 || elapsed_ms < best_time_ms) {
            best_time_ms = elapsed_ms;
        }
    }

    return {
        best_time_ms,
        total_time_ms / static_cast<double>(repetitions)
    };
}

// We double N between measurements.
//
// If:
//
//     T(N) ~ N^p
//
// then:
//
//     T(2N) / T(N) ~ 2^p
//
// so:
//
//     p = log2(slowdown)
//
// A direct N-body solver should stay close to p = 2 even when constant
// factors change because of pairwise evaluation or memory layout.
double calculate_scaling_exponent(
    double slowdown
)
{
    return std::log2(slowdown);
}

void print_result(
    const char* name,
    const BenchmarkResult& result,
    double previous_average_ms
)
{
    std::cout
        << "  " << name << '\n'
        << "    best      : "
        << result.best_time_ms
        << " ms\n"
        << "    average   : "
        << result.average_time_ms
        << " ms\n";

    if (previous_average_ms > 0.0) {

        const double slowdown =
            result.average_time_ms
            / previous_average_ms;

        const double scaling_exponent =
            calculate_scaling_exponent(
                slowdown
            );

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

    constexpr std::size_t repetitions = 10;

    std::cout
        << "\n"
        << "============================================================\n"
        << " FastPhysics - Gravity Optimization Benchmark\n"
        << "============================================================\n\n"
        << "Repetitions : "
        << repetitions
        << "\n\n"
        << "Reference AoS : N * (N - 1) interactions\n"
        << "Reference SoA : N * (N - 1) interactions\n"
        << "Pairwise AoS  : N * (N - 1) / 2 pairs\n"
        << "Pairwise SoA  : N * (N - 1) / 2 pairs\n\n"
        << "All implementations remain O(N^2).\n\n";

    double previous_reference_aos = 0.0;
    double previous_reference_soa = 0.0;
    double previous_pairwise_aos = 0.0;
    double previous_pairwise_soa = 0.0;

    for (const auto particle_count : particle_counts) {

        // Generate one physical system and derive the SoA version from it.
        // This keeps all four benchmarks based on exactly the same data.
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
            << "N = "
            << particle_count
            << "\n\n";

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
            "Pairwise AoS",
            pairwise_aos,
            previous_pairwise_aos
        );

        print_result(
            "Pairwise SoA",
            pairwise_soa,
            previous_pairwise_soa
        );

        // Effect of changing only the memory layout on the reference solver.
        const double soa_speedup =
            reference_aos.average_time_ms
            / reference_soa.average_time_ms;

        // Effect of pairwise evaluation while keeping AoS.
        const double pairwise_speedup =
            reference_aos.average_time_ms
            / pairwise_aos.average_time_ms;

        // Effect of pairwise evaluation while keeping SoA.
        const double pairwise_on_soa_speedup =
            reference_soa.average_time_ms
            / pairwise_soa.average_time_ms;

        // Effect of changing to SoA after pairwise optimization.
        const double soa_on_pairwise_speedup =
            pairwise_aos.average_time_ms
            / pairwise_soa.average_time_ms;

        // Total improvement relative to the original Reference AoS solver.
        const double combined_speedup =
            reference_aos.average_time_ms
            / pairwise_soa.average_time_ms;

        std::cout
            << "  Speedups\n"
            << "    SoA on reference      : "
            << soa_speedup
            << "x\n"
            << "    Pairwise on AoS       : "
            << pairwise_speedup
            << "x\n"
            << "    Pairwise on SoA       : "
            << pairwise_on_soa_speedup
            << "x\n"
            << "    SoA on pairwise       : "
            << soa_on_pairwise_speedup
            << "x\n"
            << "    Combined vs original  : "
            << combined_speedup
            << "x\n\n";

        previous_reference_aos =
            reference_aos.average_time_ms;

        previous_reference_soa =
            reference_soa.average_time_ms;

        previous_pairwise_aos =
            pairwise_aos.average_time_ms;

        previous_pairwise_soa =
            pairwise_soa.average_time_ms;
    }

    return 0;
}
