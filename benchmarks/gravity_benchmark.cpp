#include "fastphysics/gravity.hpp"
#include "fastphysics/scenarios.hpp"

#include <chrono>
#include <cmath>
#include <cstddef>
#include <iostream>

namespace {

struct BenchmarkResult {
    double best_time_ms{};
    double average_time_ms{};
};

// The direct N-body solver computes the interaction of every particle
// with every other particle.
//
// For N particles:
//
//     interactions = N * (N - 1)
//                  = N^2 - N
//
// For large N, the N^2 term dominates, so the solver is O(N^2).
//
// If we double the number of particles:
//
//     T(N)  ~ C * N^2
//     T(2N) ~ C * (2N)^2
//           ~ 4 * T(N)
//
// Therefore, when N doubles, we expect the runtime to increase by
// approximately 4x.
//
// The constant C represents the actual cost of the computation on the
// machine. Optimizations can reduce C and make the program faster while
// the algorithm still remains O(N^2).
BenchmarkResult benchmark_gravity(
    std::size_t particle_count,
    std::size_t repetitions
)
{
    auto particles =
        fastphysics::make_benchmark_system(
            particle_count
        );

    constexpr double gravitational_constant = 1.0;
    constexpr double softening = 0.01;

    // Warm-up run. We do not measure it.
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

    const double average_time_ms =
        total_time_ms / repetitions;

    return {
        best_time_ms,
        average_time_ms
    };
}

// Particle count doubles between measurements.
//
// If:
//
//     T(N) ~ N^p
//
// then doubling N gives:
//
//     slowdown = 2^p
//
// therefore:
//
//     p = log2(slowdown)
//
// Examples:
//
//     slowdown ~ 2x -> p ~ 1 -> O(N)
//     slowdown ~ 4x -> p ~ 2 -> O(N^2)
//     slowdown ~ 8x -> p ~ 3 -> O(N^3)
double calculate_scaling_exponent(
    double slowdown
)
{
    return std::log2(slowdown);
}

}

int main()
{
    constexpr std::size_t particle_counts[]{
        100,
        200,
        400,
        800,
        1600,
        3200
    };

    constexpr std::size_t repetitions = 10;

    std::cout
        << "\n"
        << "============================================================\n"
        << " FastPhysics - Direct N-body Gravity Benchmark\n"
        << "============================================================\n\n"
        << "Repetitions : " << repetitions << '\n'
        << "Expected    : O(N^2)\n\n"
        << "Each test doubles the number of particles.\n"
        << "For O(N^2), we expect about 4x more runtime and p close to 2.\n\n";

    double previous_average_ms = 0.0;

    for (const auto particle_count : particle_counts) {

        const BenchmarkResult result =
            benchmark_gravity(
                particle_count,
                repetitions
            );

        std::cout
            << "N = " << particle_count << '\n'
            << "  best      : "
            << result.best_time_ms
            << " ms\n"
            << "  average   : "
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
                << "  slowdown  : "
                << slowdown
                << "x\n"
                << "  scaling p : "
                << scaling_exponent
                << '\n';
        }

        std::cout << '\n';

        previous_average_ms =
            result.average_time_ms;
    }

    std::cout
        << "Expected for quadratic scaling:\n"
        << "  slowdown  -> about 4x\n"
        << "  scaling p -> about 2\n\n";

    return 0;
}