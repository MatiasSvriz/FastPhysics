#include "fastphysics/gravity.hpp"
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

// Both gravity solvers use the same arguments.
// This lets us pass either implementation to the same benchmark function.
using GravitySolver = void (*)(
    std::vector<fastphysics::Particle>&,
    double,
    double
);

struct Solver {
    const char* name;
    GravitySolver function;
};

// Both direct N-body solvers are O(N^2).

// Reference solver:
//     N * (N - 1) interactions

// Pairwise solver:
//     N * (N - 1) / 2 unique pairs

// The pairwise solver does about half as many pair evaluations, but both
// still grow with N^2. Dividing the work by two changes the constant factor,
// not the overall complexity.

// If T(N) ~ C * N^2
// then doubling N gives T(2N) ~ C * (2N)^2 ~ 4 * T(N)
// So both solvers should approach a 4x slowdown when N doubles.
// The pairwise solver should simply have a smaller constant C.
BenchmarkResult benchmark_gravity(
    GravitySolver solver,
    const std::vector<fastphysics::Particle>& initial_particles,
    std::size_t repetitions
)
{
    // Give each solver its own copy of the same system.
    // The copy happens outside the timed section.
    auto particles = initial_particles;

    constexpr double gravitational_constant = 1.0;
    constexpr double softening = 0.01;

    // Run once before measuring to reduce first-run effects such as cold
    // caches or other startup overhead.
    solver(
        particles,
        gravitational_constant,
        softening
    );

    double total_time_ms = 0.0;
    double best_time_ms = 0.0;

    for (std::size_t repetition = 0;
         repetition < repetitions;
         ++repetition) {

        // steady_clock is monotonic, so it is suitable for measuring
        // elapsed time.
        const auto start =
            std::chrono::steady_clock::now();

        solver(
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

        // Start with the first result, then keep the fastest run.
        if (repetition == 0 || elapsed_ms < best_time_ms) {
            best_time_ms = elapsed_ms;
        }
    }

    const double average_time_ms =
        total_time_ms
        / static_cast<double>(repetitions);

    return {
        best_time_ms,
        average_time_ms
    };
}

// We double the number of particles between measurements.
// This lets us estimate how fast runtime grows with N.

// If T(N) ~ N^p
// then doubling N gives T(2N) / T(N) ~ 2^p
// We will call this ratio the slowdown, so slowdown ~ 2^p
// So p = log2(slowdown)

// For example, 
// - slowdown ~ 2x -> p ~ 1 -> O(N)
// - slowdown ~ 4x -> p ~ 2 -> O(N^2)
// - slowdown ~ 8x -> p ~ 3 -> O(N^3)

// Our gravity solvers are O(N^2), so we expect p to get close to 2.
double calculate_scaling_exponent(
    double slowdown
)
{
    return std::log2(slowdown);
}

// Print the timing for one solver.

// previous_average_ms is the result for the previous particle count.
// Since N doubles each time, we can use both values to calculate the
// slowdown and estimate the scaling exponent.
void print_result(
    const char* solver_name,
    const BenchmarkResult& result,
    double previous_average_ms
)
{
    std::cout
        << "  " << solver_name << '\n'
        << "    best      : "
        << result.best_time_ms
        << " ms\n"
        << "    average   : "
        << result.average_time_ms
        << " ms\n";

    // The first measurement has no previous result to compare with.
    if (previous_average_ms > 0.0) {

        // Since N doubles, an O(N^2) solver should approach a 4x slowdown.
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
    // Double N each time so we can study how runtime scales.
    constexpr std::array<std::size_t, 6> particle_counts{
        100,
        200,
        400,
        800,
        1600,
        3200
    };

    constexpr std::size_t repetitions = 10;

    // The benchmark does not care which implementation it receives.
    // New gravity solvers can be added here later.
    constexpr std::array<Solver, 2> solvers{{
        {
            "Reference solver",
            fastphysics::compute_accelerations
        },
        {
            "Pairwise solver",
            fastphysics::compute_accelerations_pairwise
        }
    }};

    std::cout
        << "\n"
        << "============================================================\n"
        << " FastPhysics - Gravity Solver Benchmark\n"
        << "============================================================\n\n"
        << "Repetitions : "
        << repetitions
        << '\n'
        << "Expected    : O(N^2)\n\n"
        << "Reference solver evaluates N * (N - 1) ordered interactions.\n"
        << "Pairwise solver evaluates N * (N - 1) / 2 unique pairs.\n\n"
        << "Both remain O(N^2), but pairwise should reduce runtime.\n"
        << "When N doubles, both should approach a 4x slowdown.\n\n";

    // Store the previous average separately for each solver.
    std::array<double, solvers.size()> previous_average_ms{};

    for (const auto particle_count : particle_counts) {

        // Create the system once so every solver starts from the same data.
        const auto initial_particles =
            fastphysics::make_benchmark_system(
                particle_count
            );

        std::array<BenchmarkResult, solvers.size()> results{};

        std::cout
            << "N = "
            << particle_count
            << "\n\n";

        // Run every solver using the same benchmark code.
        for (std::size_t i = 0;
             i < solvers.size();
             ++i) {

            results[i] =
                benchmark_gravity(
                    solvers[i].function,
                    initial_particles,
                    repetitions
                );

            print_result(
                solvers[i].name,
                results[i],
                previous_average_ms[i]
            );

            previous_average_ms[i] =
                results[i].average_time_ms;
        }

        // Compare the original solver against the pairwise version.
        // speedup = reference time / pairwise time
        // A value above 1 means pairwise is faster.
        // For example, 
        // - 10 ms / 6 ms = 1.67x speedup
        const double best_speedup =
            results[0].best_time_ms
            / results[1].best_time_ms;

        const double average_speedup =
            results[0].average_time_ms
            / results[1].average_time_ms;

        std::cout
            << "  Pairwise speedup\n"
            << "    best      : "
            << best_speedup
            << "x\n"
            << "    average   : "
            << average_speedup
            << "x\n\n";
    }

    std::cout
        << "Expected behaviour:\n"
        << "  scaling slowdown -> about 4x\n"
        << "  scaling p        -> about 2\n"
        << "  pairwise speedup -> greater than 1x\n\n";

    return 0;
}