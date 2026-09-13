#pragma once

#include "fastphysics/particle.hpp"

#include <vector>

namespace fastphysics {

// Creates a deterministic two-body system with equal masses and symmetric initial conditions around the origin.
std::vector<Particle> make_two_body_system();

// Creates a deterministic particle system intended for performance benchmarks.
//
// Particles are placed along the x-axis with unit spacing, zero initial
// velocity and unit mass. The deterministic layout ensures that repeated
// benchmark runs operate on the same input data.
std::vector<Particle> make_benchmark_system(
    std::size_t particle_count
);

}